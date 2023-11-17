# Use XLA interactive_graphviz to visualize JAX models
This doc explains how to visualize forward and backward path of JAX models

## Prepare JAX model
Example of the model is [softmax.py](softmax.py).

It has Add, Softmax and Loss (as mean) operations.

forward function computation is saved as `softmax_fwd.hlo.txt`

forward and backward (train_forward) function computation is saved as `softmax_train.hlo.txt`

## Build interactive_graphviz

To build interactive_graphviz in TF or OpenXLA projects

```bash
bazel build //tensorflow/compiler/xla/tools:interactive_graphviz

# or

bazel build //xla/tools:interactive_graphviz
```

## Generate HTML+SVG from hlo

To Visualize hlo.txt files we can use interactive_graphviz

```bash
$TF/bazel-bin/tensorflow/compiler/xla/tools/interactive_graphviz \
  --hlo_text=softmax_train.hlo.txt

# or

$XLA/bazel-bin/xla/tools/interactive_graphviz \
  --hlo_text=softmax_train.hlo.txt
```

### Basic interactive_graphviz commands

To get list of Entry computations.
```
list computations
```

To visualize particular Entry computation - just type its `name.id`

It will show generated HTML file path (it will be in /tmp folder)


## Start WEB Server on Linux box

To start webserver on Linux box looking at /tmp

```bash
cd /tmp
python3 -m http.server 8080
```

## SSH Tunnel

To start ssh tunnel to access Linux web server on Mac

```bash
ssh -NL 8080:localhost:8080 user@linux_host
```

Or use VSCode, Connect to Host, Ports panel, Forward a Port (in View - Appearance - Panel (⌘+J))

## Open graph HTML URL in browser

To open interactive_graphviz generated HTML in browser use the following url example

```bash
http://localhost:8080/interactive_graphviz.1700255637823980.html
```

"Save SVG" link will be in the bottom left corner on the page


## Save SVG as PNG

To save SVG to PNG in hight quality we can use `cairosvg` tool

Install cairosvg
```bash
brew install python3 cairo libxml2 libxslt libffi
pip3 install cairosvg
```

To Convert SVG to PNG
```bash
cairosvg -s 2 -o softmax_train.png softmax_train.svg
```

## Derivatives calculation in Backpropagation

Derivatives for basic ops [derivatives.svg](derivatives.svg)

Chain rule - [derivatives-chain-rule.svg](derivatives-chain-rule.svg)

If one node is used multiple times downstream then calculate grads independently along each path and accumulate the resulting grads - [derivatives-one-to-many.svg](derivatives-one-to-many.svg)

[derivatives.drawio](derivatives.drawio)
