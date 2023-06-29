static const std::string vertex_shader_text = R"(
// START VERTEX
#version 110
attribute vec3 vCol;
attribute vec2 vPos;
varying vec3 color;
void main() {
    gl_Position = vec4(vPos, 0.0, 1.0);
    color = vCol;
}
// END
)";
