import numpy as np
import sounddevice as sd
import sys
import kps

fs = 44100
blocksize = 256

pan = 0.0
amp = 0.4

def pan_gains(p):
    angle = (p + 1.0) * 0.25 * np.pi
    return np.cos(angle), np.sin(angle)

gain_l, gain_r = pan_gains(pan)

osc1 = kps.KpsVoice()
osc2 = kps.KpsVoice()
osc3 = kps.KpsVoice()
osc1.set_damp(0.85)
osc2.set_damp(0.85)
osc3.set_damp(0.85)
osc1.set_freq(550.5 * 0.5)
osc2.set_freq(440.2 * 0.5)
osc3.set_freq(660.1 * 0.5)

def audio_callback(outdata, frames, time, status):
    global idx, delay

    if status:
        print(status)

    buf1 = np.empty(frames, dtype=np.float32)
    buf2 = np.empty(frames, dtype=np.float32)
    buf3 = np.empty(frames, dtype=np.float32)

    osc1.get_next_block(buf1)
    osc2.get_next_block(buf2)
    osc3.get_next_block(buf3)

    buf1 *= 0.33
    buf2 *= 0.33
    buf3 *= 0.33
    buf1 += buf2 + buf3

    outdata[:, 0] = amp * gain_l * buf1
    outdata[:, 1] = amp * gain_r * buf1

def main():
    with sd.OutputStream(
        channels=2,
        samplerate=fs,
        blocksize=blocksize,
        callback=audio_callback,
    ):
        while True:
            op = input("> ")
            if  op == "q":
                sys.exit(0)
            elif op == "a":
                osc1.excite()
            elif op == "b":
                osc2.excite()
            elif op == "ab":
                osc1.excite()
                osc2.excite()

if __name__ == "__main__":
    main()
