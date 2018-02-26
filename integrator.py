import pandas as pd
import numpy as np
from scipy import integrate, signal
import matplotlib.pyplot as plt

def quat2matrot(q):
    """fonction de convertion permettant de calculer la matrice de rotation qui permet de remettre les données dans repère inertielle et non celle du capteur

    :q: TODO
    :returns: TODO

    """
    r = q[0]
    i = q[1]
    j = q[2]
    k = q[3]


    m_r = np.ones((3, 3))

    m_r[0, 0] = 2 * (-j * j - k * k) + 1
    m_r[0, 1] = 2 * (i * j - r * k)
    m_r[0, 2] = 2 * (i * k + r * j)

    m_r[1, 0] = 2 * (i * j + r * k)
    m_r[1, 1] = 2 * (-i * i - k * k ) + 1
    m_r[1, 2] = 2 * (j * k - r * i)

    m_r[2, 0] = 2 * (i * k - r * j)
    m_r[2, 1] = 2 * (j * k + r * i)
    m_r[2, 2] = 2 * (-i * i - j * j) + 1

    return m_r

def process_all_q2mr(q0, q1, q2, q3):
    """traitement du tous les quaternions vers toutes matrices de rotations

    :q0: TODO
    :q1: TODO
    :q2: TODO
    :q3: TODO
    :returns: TODO

    """
    m_r = []
    np.array(m_r)
    for i in range(len(q1)):
        q = [q0[i], q1[i], q2[i], q3[i]]
        m_rtn = quat2matrot(q)
        m_r.append(m_rtn)

    return m_r

def rep_boitier_inertiel(Ax, Ay, Az, q0, q1, q2, q3):
    """TODO: Docstring for rep_boitier_inertiel.

    :Ax: Acc lin x
    :Ay: Acc lin y
    :Az: Acc lin z
    :q0: quat q0
    :q1: quat q1
    :q2: quat q2
    :q3: quat q3
    :returns: array Ax, Ay, Az corrigé dans l'espace

    """
    Axi = []
    Ayi = []
    Azi = []
    A= []


    m_r = process_all_q2mr(q0, q1, q2, q3)

    for i in range(len(Ax)):
        A = np.array([Ax[i], Ay[i], Az[i]])
        Ares = np.dot(m_r[i].T, A.T)
        Axi.append(Ares[0])
        Ayi.append(Ares[1])
        Azi.append(Ares[2])


    return np.array(Axi), np.array(Ayi), np.array(Azi)

def calc_offset(vect_x, vect_y, vect_z, offset_size):
    acc_x = acc_y = acc_z  = 0
    for i in range(offset_size):
        acc_x += vect_x[i]
        acc_y += vect_y[i]
        acc_z += vect_z[i]
    offset_x = acc_x/offset_size
    offset_y = acc_y/offset_size
    offset_z = acc_z/offset_size

    return offset_x, offset_y, offset_z

# g = 9.8m/s^2
C_G = 9.8
# offset to quaternion stabilization
SAMPLE_OFFSET = 350
# calibration samples (50*0.02 = 1s)
CALIBRATION_SAMPLES = 50
# Sampling at 50Hz
F_S = 50

csv = pd.read_csv("raw_data/y_acc.csv", skiprows=range(1, SAMPLE_OFFSET))
# Timestamp in seconds
timestamp = np.array(csv["timestamp"])
# Acc converted to m/s^2
acc_x = C_G*np.array(csv["ax"])
acc_y = C_G*np.array(csv["ay"])
acc_z = C_G*np.array(csv["az"])
q0 = np.array(csv["q0"])
q1 = np.array(csv["q1"])
q2 = np.array(csv["q2"])
q3 = np.array(csv["q3"])

# Corrections
[acc_xc, acc_yc, acc_zc] = rep_boitier_inertiel(acc_x, acc_y, acc_z, q0, q1, q2, q3)
acceleration_bo = np.stack([timestamp, acc_xc, acc_yc, acc_zc], axis=1)
header_a = "timestamp, a_x, a_y, a_z"
np.savetxt("./output/acceleration_bo.csv", acceleration_bo, delimiter=", ", header=header_a,
           fmt="%.3f")

offset_x, offset_y, offset_z = calc_offset(acc_xc, acc_yc, acc_zc, CALIBRATION_SAMPLES)
acc_xcc = np.subtract(acc_xc, offset_x)
acc_ycc = np.subtract(acc_yc, offset_y)
acc_zcc = np.subtract(acc_zc, offset_z)

acceleration_bf = np.stack([timestamp, acc_xcc, acc_ycc, acc_zcc], axis=1)
np.savetxt("./output/acceleration_bf.csv", acceleration_bf, delimiter=", ", header=header_a,
           fmt="%.3f")

# Low-pass filtering
f_cutoff = 7.5
#b, a = signal.butter(2, f_cutoff/F_S, 'low')
#zi = signal.lfilter_zi(b, a)
#acc_xf, _ = signal.lfilter(b, a, acc_xcc, zi=zi*acc_xcc[0])
#acc_yf, _ = signal.lfilter(b, a, acc_ycc, zi=zi*acc_ycc[0])
#acc_zf, _ = signal.lfilter(b, a, acc_zcc, zi=zi*acc_zcc[0])

#Bypassing filter
acc_xf = acc_xcc
acc_yf = acc_ycc
acc_zf = acc_zcc

acceleration = np.stack([timestamp, acc_xf, acc_yf, acc_zf], axis=1)
np.savetxt("./output/acceleration.csv", acceleration, delimiter=", ", header=header_a, fmt="%.3f")

velocity_x = np.array(integrate.cumtrapz(acc_xf, timestamp, initial=0))
velocity_y = np.array(integrate.cumtrapz(acc_yf, timestamp, initial=0))
velocity_z = np.array(integrate.cumtrapz(acc_zf, timestamp, initial=0))

velocity_bf = np.stack([timestamp, velocity_x, velocity_y, velocity_z], axis=1)
header_v = "timestamp, v_x, v_y, v_z"
np.savetxt("./output/velocity_bf.csv", velocity_bf, delimiter=", ", header=header_v, fmt="%.3f")

# High-pass filtering
b, a = signal.butter(2, f_cutoff/F_S, 'high')
zi = signal.lfilter_zi(b, a)
velocity_xf, _ = signal.lfilter(b, a, velocity_x, zi=zi*acc_xcc[0])
velocity_yf, _ = signal.lfilter(b, a, velocity_y, zi=zi*acc_ycc[0])
velocity_zf, _ = signal.lfilter(b, a, velocity_z, zi=zi*acc_zcc[0])

velocity = np.stack([timestamp, velocity_xf, velocity_yf, velocity_zf], axis=1)
np.savetxt("./output/velocity.csv", velocity, delimiter=", ", header=header_v, fmt="%.3f")

position_x = np.array(integrate.cumtrapz(velocity_xf, timestamp, initial=0))
position_y = np.array(integrate.cumtrapz(velocity_yf, timestamp, initial=0))
position_z = np.array(integrate.cumtrapz(velocity_zf, timestamp, initial=0))
position = np.stack([timestamp, position_x, position_y, position_z], axis=1)
header_p = "timestamp, p_x, p_y, p_z"
np.savetxt("./output/position.csv", position, delimiter=", ", header=header_p, fmt="%.3f")
