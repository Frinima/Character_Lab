// Rasterize the project's editable SVG icons for Unreal Texture2D import.
const path = require('path');
const sharp = require(process.argv[2]);
const root = path.resolve(__dirname, '..');
(async () => {
  for (const name of ['Chest', 'Gem', 'Scroll', 'Ore', 'Sword']) {
    await sharp(path.join(root, 'Content/UI/Icons', name + '.svg'), {density: 192})
      .png().toFile(path.join(root, 'Content/UI/Icons', name + '.png'));
    console.log('Rendered ' + name);
  }
})().catch(error => { console.error(error); process.exitCode = 1; });
