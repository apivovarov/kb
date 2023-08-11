import numpy as np
import torch
from transformers import RobertaTokenizer, RobertaModel
from torchinfo import summary

torch.set_grad_enabled(False)

class RobertaTraceWrapper(torch.nn.Module):
    def __init__(self, model):
        super().__init__()
        self.model = model
    def forward(self, x):
        out = self.model(x)
        return (out['last_hidden_state'], out['pooler_output'])

tokenizer = RobertaTokenizer.from_pretrained('roberta-large')
model = RobertaModel.from_pretrained('roberta-large').cuda().eval()
wrap_model = RobertaTraceWrapper(model).cuda().eval()

text = "Replace me by any text you'd like."
encoded_input = tokenizer(text, return_tensors='pt')
input_ids = encoded_input['input_ids'].cuda()
print(f"{input_ids.shape=}")

#summary(model, input_ids.shape)

res = wrap_model(input_ids)

#print(f"res: {res}")
print(f"{res[0].shape}")
print(f"{res[1].shape}")

print("Converting to ONNX")
input_names = ["input_ids"]
output_names = ["last_hidden_state", "pooler_output"]
dynamic_axes = {
        "input_ids": {0: "batch", 1: "token"},
        "last_hidden_state": {0: "batch", 1: "token"},
        "pooler_output": {0: "batch"}
}

#y = model(input_ids)
torch.onnx.export(
    wrap_model,
    input_ids,
    "roberta-large.onnx",
    verbose=True,
    input_names=input_names,
    output_names=output_names,
    dynamic_axes=dynamic_axes,
)
print("The model was converted to ONNX")
