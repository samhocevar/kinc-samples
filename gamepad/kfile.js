const project = new Project('gamepad');

await project.addProject('../../kinc/');

project.addFile('src/**');
project.addFile('shaders/**');
project.setDebugDir('deployment');

project.flatten();

resolve(project);
