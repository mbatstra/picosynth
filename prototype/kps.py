import numpy as np

fs = 44100
# freq = 110.0
# decay = 0.993
# delay_len = int(fs / freq)
# noise = np.random.uniform(-1.0, 1.0, delay_len).astype(np.float32)
# 
# lp_coef = 0.35
# lp_state = 0.0
# for i in range(delay_len):
#     lp_state = (1.0 - lp_coef) * noise[i] + lp_coef * lp_state
#     noise[i] = lp_state
# 
# delay = noise
# idx = 0
# 
# def do_block(buf):
#     global idx, delay
#     for i in range(len(buf)):
#         buf[i] = delay[idx]
#         next_idx = (idx + 1) % delay_len
#         delay[idx] = decay * 0.5 * (delay[idx] + delay[next_idx])
#         idx = next_idx

class KpsVoice:
    fmin = 20

    def __init__(self):
        self.idx = 0
        self.decay = 0.993
        self.freq = 440
        self.delaysize = int(fs / self.fmin)
        self.delay = np.zeros(int(fs / self.fmin)).astype(np.float32)

    def set_damp(self, coef):
        state = 0.0
        self.delay = np.random.uniform(-1.0, 1.0, int(fs / self.fmin)).astype(np.float32)
        for i in range(len(self.delay)):
            state = (1.0 - coef) * self.delay[i] + coef * state
            self.delay[i] = state

    def set_freq(self, f):
        if f < self.fmin:
            raise ValueError
        self.freq = f
        self.delaysize = int(fs / f)

    def excite(self):
        self.delay = np.random.uniform(-1.0, 1.0, int(fs / self.fmin)).astype(np.float32)

    def get_next_block(self, buf):
        for i in range(len(buf)):
            buf[i] = self.delay[self.idx]
            next_idx = (self.idx + 1) % self.delaysize
            self.delay[self.idx] = self.decay * 0.5 * (self.delay[self.idx] + self.delay[next_idx] + np.random.uniform(-0.05, 0.05))
            self.idx = next_idx
