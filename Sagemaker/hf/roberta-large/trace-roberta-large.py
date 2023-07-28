import torch
from transformers import RobertaTokenizer, RobertaModel

torch.set_grad_enabled(False)

class RobertaTraceWrapper(torch.nn.Module):
    def __init__(self, model):
        super().__init__()
        self.model = model
    def forward(self, x):
        out = self.model(x)
        return (out['last_hidden_state'], out['pooler_output'])


with torch.inference_mode():
  tokenizer = RobertaTokenizer.from_pretrained('roberta-large')
  model = RobertaModel.from_pretrained('roberta-large')
  wrap_model = RobertaTraceWrapper(model)


with torch.inference_mode():
  text = "Replace me by any text you'd like."
  encoded_input = tokenizer(text, return_tensors='pt')
  input_ids = encoded_input['input_ids']
  last_hidden_state, pooler_output = wrap_model(input_ids)


with torch.inference_mode():
  traced_model = torch.jit.trace(wrap_model, input_ids)

with torch.inference_mode():
  last_hidden_state2, pooler_output2 = traced_model(input_ids)

with torch.inference_mode():
  torch.jit.save(traced_model, "roberta-large.pt")

print("Traced model was saved")
