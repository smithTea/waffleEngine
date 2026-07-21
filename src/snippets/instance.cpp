void Before(){
    Mesh cube = MakeCube();

    InstanceBuffer floor;
    floor.GenerateMatrices(100 , 100, 10);

    cube.Upload();
    floor.Upload();
    cube.AttachInstanceBuffer(floor);
}

void Update(){
   m_Renderer.DrawInstanced(cube, trigShader, floor);
}

