static const std::string fragment_shader_text = R"(
// START FRAGMENT
#version 110
varying vec3 color;
void main() {
    gl_FragColor = vec4(color, 1.0);
}
// END
)";
