import torch
from transformers import RobertaTokenizer, RobertaModel

torch.set_grad_enabled(False)

# Add .half() to the models below to get faster inference

class RobertaTraceWrapper(torch.nn.Module):
    def __init__(self, model):
        super().__init__()
        self.model = model
    def forward(self, x):
        out = self.model(x)
        return (out['last_hidden_state'], out['pooler_output'])


with torch.inference_mode():
  tokenizer = RobertaTokenizer.from_pretrained('roberta-large')
  model = RobertaModel.from_pretrained('roberta-large').cuda() #.half()
  wrap_model = RobertaTraceWrapper(model).cuda().eval() #.half()


with torch.inference_mode():
  text = "Replace me by any text you'd like."
  encoded_input = tokenizer(text, return_tensors='pt')
  input_ids = encoded_input['input_ids'].cuda()
  last_hidden_state, pooler_output = wrap_model(input_ids)
  traced_model = torch.jit.trace(wrap_model, input_ids).cuda() #.half()
  last_hidden_state2, pooler_output2 = traced_model(input_ids)

import time
import numpy as np

with torch.inference_mode():
    N = 100
    for i in range(N):
      x = torch.randint(low=0,high=500,size=(1,12), dtype=torch.int64).cuda()
      out = traced_model(x)
      #out = wrap_model(x)

with torch.inference_mode():
    N = 1000
    TT = []
    for i in range(N):
      x = torch.randint(low=0,high=500,size=(1,12), dtype=torch.int64).cuda()
      t0 = time.time()
      out = traced_model(x)
      #out = wrap_model(x)
      t1 = time.time()
      TT.append(t1-t0)

avg_time = np.mean(TT) * 1000
print(f"avg time: {avg_time} ms")
