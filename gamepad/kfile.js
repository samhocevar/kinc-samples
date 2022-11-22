const project = new Project('gamepad');

await project.addProject('../../kinc/');

project.addFile('*.c');
project.addFile('*.glsl');
project.setDebugDir('deployment');

project.flatten();

resolve(project);
