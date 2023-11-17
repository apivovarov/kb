import jax
from jax import Array
import jax.numpy as jnp

def init_params(key: Array, shape) -> Array:
    return jax.random.normal(key, shape).astype(jax.dtypes.bfloat16)

def softmax(x):
    mx = x.max(axis=-1, keepdims=True)
    mx = jax.lax.stop_gradient(mx)
    x = x - mx
    ex=jnp.exp(x) # exp grad is ex
    denom = ex.sum(axis=-1, keepdims=True)
    y = ex / denom
    return y

def forward(params: Array, x: Array) -> Array:
    x = x + params
    y = softmax(x)
    # y = jax.nn.softmax(x)
    # unnormalized = jnp.exp(x - lax.stop_gradient(x.max(axis, keepdims=True)))
    # return unnormalized / unnormalized.sum(axis, keepdims=True)
    return y

@jax.value_and_grad
def train_forward(params, seq):
  out = forward(params, seq)
  loss = jnp.mean(out)
  return loss

@jax.jit
def train_step(params, seq):
  loss, grads = train_forward(params, seq)
  return loss, grads

key = jax.random.PRNGKey(42)
key, init_key = jax.random.split(key)
shape=(4,2,2048,2048)
params = init_params(key=init_key, shape=shape)

key, input_key = jax.random.split(key)
input_tensor = jax.random.normal(input_key, shape).astype(jax.dtypes.bfloat16)

loss, grads = train_step(params, input_tensor)


### Save as HLO
y1 = jax.xla_computation(forward)(params,input_tensor)
with open("softmax_fwd.hlo.txt", "w") as f:
    f.write(y1.as_hlo_text())

y2 = jax.xla_computation(train_forward)(params,input_tensor)
with open("softmax_train.hlo.txt", "w") as f:
    f.write(y2.as_hlo_text())

