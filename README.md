### Use Shader to render two triangles independently
* Import code snippet

``` cpp
	glBindVertexArray(VAO2);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
```

![Shader Triangles](shader_two_triangles.png)
