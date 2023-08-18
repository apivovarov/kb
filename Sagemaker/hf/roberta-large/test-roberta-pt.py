import numpy as np
import torch
from transformers import RobertaTokenizer, RobertaModel
import json

with open("sizes.json") as f:
    sizes = json.load(f)


torch.set_grad_enabled(False)

# Add .half() to the models below to get faster inference

class RobertaTraceWrapper(torch.nn.Module):
    def __init__(self, model):
        super().__init__()
        self.model = model
    def forward(self, inp, am):
        out = self.model(inp, am)
        return (out['last_hidden_state'], out['pooler_output'])


tokenizer = RobertaTokenizer.from_pretrained('roberta-large')
model = RobertaModel.from_pretrained('roberta-large').cuda().eval()
#model = RobertaModel.from_pretrained('roberta-large').cuda().half().eval()
wrap_model = RobertaTraceWrapper(model).cuda().eval()
#wrap_model = RobertaTraceWrapper(model).cuda().half().eval()


with torch.no_grad():
  text = "Replace me by any text you'd like."
  encoded_input = tokenizer(text, return_tensors='pt')
  input_ids = encoded_input['input_ids'].cuda()
  am = encoded_input['attention_mask'].cuda()
  last_hidden_state, pooler_output = wrap_model(input_ids, am)
  #traced_model = torch.jit.trace(wrap_model, (input_ids, am)).eval().cuda()#.half()
  traced_model = torch.jit.script(wrap_model, example_inputs=(input_ids, am)).eval().cuda()#.half()
  last_hidden_state2, pooler_output2 = traced_model(input_ids, am)

exit()

N = 100
with torch.inference_mode():
    for i in range(N):
      sz = sizes[i%1000]
      print(i, sz)
      inp = torch.randint(low=0,high=500,size=(1,sz), dtype=torch.int64).cuda()
      am = torch.ones((1,sz), dtype=torch.int64).cuda()
      out = traced_model(inp, am)
      #out = wrap_model(inp, am)

N = 1000
TT = []
start = torch.cuda.Event(enable_timing=True)
end = torch.cuda.Event(enable_timing=True)
with torch.inference_mode():
    for i in range(N):
      sz = sizes[i%1000]
      print(i, sz)
      inp = torch.randint(low=0,high=500,size=(1,sz), dtype=torch.int64)
      am = torch.ones((1,sz), dtype=torch.int64)
      start.record()
      inp=inp.cuda()
      am=am.cuda()
      out = traced_model(inp, am)
      #out = wrap_model(inp, am)
      lhs=out[0]
      pool=out[1]
      end.record()
      torch.cuda.synchronize()
      TT.append(start.elapsed_time(end))

avg_time = np.mean(TT)
print(f"avg time: {avg_time} ms")
