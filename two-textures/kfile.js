const project = new Project('Textures');

await project.addProject('../../kinc/');

project.addFile('src/**');
project.addFile('shaders/**');
project.setDebugDir('deployment');

project.flatten();

resolve(project);
