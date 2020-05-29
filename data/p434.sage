###############################################################################
#
# Script to reproduce the measurements of section
# 5.2 in the paper. It first prints the cycle counts
# of the actual measurements for w = 2^16, 2^18, 2^20,
# as displayed in Table 7. These are taken from the
# files
#
#   'raw_data/p434_atomkohlesike_full_atk_False_hag_False_precomp_16',
#   'raw_data/p434_atomkohle_w_18_sike_full_atk_False_hag_False_precomp_16',
#   'raw_data/p434_atomkohle_w_20_sike_full_atk_False_hag_False_precomp_16.
#
# Then it prints the three fitted lines z_16, z_18
# and z_20. Finally it prints the expected values
# for the total runtime of the function for different
# values of w. All formulas used here as identical to
# those in the paper.
#
###############################################################################

reset()

import numpy as np
import matplotlib.pyplot as plt
import json

delta = 16 # precomputation depth

def zeta(e):
    return (1/2)*((e-2)*log(e-2,2) + (e-1)*log(e-1,2))

def sigmad(delta,e):
    return (1/2)*(e-2-delta)*log(e-2-delta,2)*( 1/((e-2)*log(e-2,2)) + 1/((e-1)*log(e-1,2)) )

# Load the data from file
def gen_data(s,first_line,last_line,first_it,w):
    with open(s) as f:
        data = map(json.loads, f.readlines()[:-1]) # last line of data not fully dumped yet

    # xs = set size
    xs = [ np.exp2(first_it+2*j-1) for j in range(first_line,last_line) ]
    x = np.array(xs)
    ys = [ np.float64(data[j]['v']['cycles']/(zeta(first_it+2*j)*sigmad(delta,first_it+2*j)*data[j]['v']['dist_cols'])) 
            for j in range(first_line,last_line) ]
    y = np.array(ys)

    zp,res,_,_,_ = np.polyfit(np.sqrt(x),y,1,full=True)
    z = np.poly1d(zp)

    xp = np.linspace(0, np.sqrt(xs[last_line-first_line-1]), 100)

    return x,y,xs,ys,xp,z

first_line16 = 5
last_line16 = 13
first_it16 = 18
w16 = 16.
x16,y16,xs16,ys16,xp16,z16 = gen_data('raw_data/p434_atomkohlesike_full_atk_False_hag_False_precomp_16',
                    first_line16,
                    last_line16,
                    first_it16,
                    w16)

print("w = 2^16")
for y in ys16:
    print('%.0f' % float(y/1000))

first_line18 = 1
last_line18 = 8
first_it18 = 28
w18 = 18.
x18,y18,xs18,ys18,xp18,z18 = gen_data('raw_data/p434_atomkohle_w_18_sike_full_atk_False_hag_False_precomp_16',
                    first_line18,
                    last_line18,
                    first_it18,
                    w18)

print("w = 2^18")
for y in ys18:
    print('%.0f' % float(y/1000))

first_line20 = 2
last_line20 = 8
first_it20 = 28
w20 = 20.
x20,y20,xs20,ys20,xp20,z20 = gen_data('raw_data/p434_atomkohle_w_20_sike_full_atk_False_hag_False_precomp_16',
                    first_line20,
                    last_line20,
                    first_it20,
                    w20)

print("w = 2^20")
for y in ys20:
    print('%.0f' % float(y/1000))

ax = plt.subplot(111)
ax.spines['right'].set_visible(False)
ax.spines['top'].set_visible(False)
ax.yaxis.set_ticks_position('left')
ax.xaxis.set_ticks_position('bottom')

plt.xlabel('log_2(sqrt(N))')
plt.ylabel("log_2(t/c)")
#plt.xscale("log", basex=2)
#plt.yscale("log", basey=2)
plt.xlim(xmax=1.1*np.sqrt(xs16[last_line16-first_line16-1]))
plt.ylim(ymax=1.1*float(ys16[last_line16-first_line16-1]))
plt.plot(np.sqrt(x16), y16, '.', xp16, z16(xp16), '--',
         np.sqrt(x18), y18, '*', xp18, z18(xp18), '--',
         np.sqrt(x20), y20, '+', xp20, z20(xp20), '--'
        )

# Direct this to the desired folder
#plt.savefig("../../../paper/p434.png")
#plt.show()

print("")
print("z16:", z16)
print("z18:", z18)
print("z20:", z20)

e = 108.
num_cores = 28
print("\nw = 2^16")
print("Full algorithm: (measured + expected)")
print( zeta(e)*sigmad(delta,e)*z16(np.sqrt(np.exp2(e-1)))*np.exp2(e-1)/2 )
print( float(sigmad(delta,e)*2**22*2.5*np.sqrt((np.exp2(e-1)**3)/np.exp2(w16))) / num_cores )
print("")
print("Full algorithm log: (measured + expected)")
print( log(float(zeta(e)*sigmad(delta,e)*z16(np.sqrt(np.exp2(e-1)))*np.exp2(e-1)/2),2.) )
print( log(float(sigmad(delta,e)*2**22*2.5*np.sqrt((np.exp2(e-1)**3)/np.exp2(w16)) / num_cores),2.) )

e = 108.
num_cores = 28
print("\nw = 2^18")
print("Full algorithm: (measured + expected)")
print( zeta(e)*sigmad(delta,e)*z18(np.sqrt(np.exp2(e-1)))*np.exp2(e-1)/2 )
print( float(sigmad(delta,e)*2**22*2.5*np.sqrt((np.exp2(e-1)**3)/np.exp2(w18))) / num_cores )
print("")
print("Full algorithm log: (measured + expected)")
print( log(float(zeta(e)*sigmad(delta,e)*z18(np.sqrt(np.exp2(e-1)))*np.exp2(e-1)/2),2.) )
print( log(float(sigmad(delta,e)*2**22*2.5*np.sqrt((np.exp2(e-1)**3)/np.exp2(w18)) / num_cores),2.) )

e = 108.
num_cores = 28
print("\nw = 2^20")
print("Full algorithm: (measured + expected)")
print( zeta(e)*sigmad(delta,e)*z20(np.sqrt(np.exp2(e-1)))*np.exp2(e-1)/2 )
print( float(sigmad(delta,e)*2**22*2.5*np.sqrt((np.exp2(e-1)**3)/np.exp2(w20))) / num_cores )
print("")
print("Full algorithm log: (measured + expected)")
print( log(float(zeta(e)*sigmad(delta,e)*z20(np.sqrt(np.exp2(e-1)))*np.exp2(e-1)/2),2.) )
print( log(float(sigmad(delta,e)*2**22*2.5*np.sqrt((np.exp2(e-1)**3)/np.exp2(w20)) / num_cores),2.) )
