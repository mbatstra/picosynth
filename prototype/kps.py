import numpy as np
import sounddevice as sd

fs = 44100
blocksize = 256

freq = 110.0
amp = 0.4
decay = 0.995

delay_len = int(fs / freq)

noise = np.random.uniform(-1.0, 1.0, delay_len).astype(np.float32)

lp_coef = 0.35
lp_state = 0.0
for i in range(delay_len):
    lp_state = (1.0 - lp_coef) * noise[i] + lp_coef * lp_state
    noise[i] = lp_state

delay = noise
idx = 0

def audio_callback(outdata, frames, time, status):
    global idx, delay
    if status:
        print(status)

    out = np.empty(frames, dtype=np.float32)

    for i in range(frames):
        out[i] = delay[idx]
        next_idx = (idx + 1) % delay_len
        delay[idx] = decay * 0.5 * (delay[idx] + delay[next_idx])
        idx = next_idx

    outdata[:, 0] = amp * out

if __name__ == "__main__":
    with sd.OutputStream(
        channels=1,
        samplerate=fs,
        blocksize=blocksize,
        callback=audio_callback,
    ):
        while True:
            pass
