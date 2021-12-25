### Use Shader to render two triangles independently

### Setup Vertex Array Object (VAO) and Vertex Buffer Object (VBO) 
    * First Triangle
``` cpp
    unsigned int VBO1, VAO1;
    //            |      +-> vertex array object
    //            +-> vertex buffer object
    glGenVertexArrays(1, &VAO1);
    //                |
    //                + -> the number of vertex array object names to generate
    //
    glGenBuffers(1, &VBO1);

    glBindVertexArray(VAO1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVert), triVert, GL_STATIC_DRAW);

    //                       + -> specify number of components per generic vertex attribute
    //                       ↓ 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    //                                                 ↑ 
    //                                                 + next vertex shift 6 floats
    //
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    //                                                 ↑
    //                                                 + -> next color shift 6 floats
    //
    glEnableVertexAttribArray(1);
```

### Bind Vertex Array and draw 
``` cpp
	glBindVertexArray(VAO1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
```

    * Second Triangle
``` cpp
    unsigned int VBO2, VAO2;
    //            |      +-> vertex array object
    //            +-> vertex buffer object
    glGenVertexArrays(1, &VAO2);
    //                |
    //                + -> the number of vertex array object names to generate
    //
    glGenBuffers(1, &VBO2);

    glBindVertexArray(VAO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triVert), triVert, GL_STATIC_DRAW);

    //                       + -> specify number of components per generic vertex attribute
    //                       ↓ 
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)0);
    //                                                 ↑ 
    //                                                 + next vertex shift 6 floats
    //
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (void*)(3*sizeof(float)));
    //                                                 ↑
    //                                                 + -> next color shift 6 floats
    //
    glEnableVertexAttribArray(1);
```

### Bind Vertex Array and draw 
``` cpp
	glBindVertexArray(VAO1);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 3);
```

![Shader Triangles](shader_two_triangles.png)
