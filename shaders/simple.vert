#version 120

invariant gl_Position;
attribute vec2 position;
attribute vec2 offset;

void main(void) {
  gl_Position = vec4(position.x, position.y, 0.0, 1.0);
}
