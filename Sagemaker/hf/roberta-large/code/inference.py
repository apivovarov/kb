import logging
import torch
import json
import numpy as np

def input_fn(input_data, content_type):
    logging.debug(f"Run input_fn. {input_data=}, {content_type=}")
    assert content_type == 'application/json', "Unexpected {content_type=}. Need application/json"
    data = json.loads(input_data)
    np_array = np.array(data, dtype="int64")
    tensor = torch.LongTensor(np_array).cuda()
    logging.info(f"input {tensor.dtype=}, {tensor.shape=}")
    return tensor

