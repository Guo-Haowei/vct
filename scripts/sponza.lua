local windowScale = 1;

-- graphics
Dvar.SetInt('r_voxel_size', 64);
Dvar.SetInt('r_shadowRes', 1024 * 4);

-- camera
Dvar.SetVec3('cam_pos', -5.0, 5.0, 0.0);
-- Dvar.SetVec3('cam_pos', -5.6, 0.83, 2.72);
Dvar.SetFloat('cam_fov', 60.0);