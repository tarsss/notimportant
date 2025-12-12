#version 330

in vec2             v2f_uv;
uniform sampler2D   tex;
uniform vec4        color;
out vec4            frag_color;

void main()
{
    vec4 tex = texture(tex, v2f_uv);
    frag_color = tex * color;
}