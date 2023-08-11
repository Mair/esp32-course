1. to install and run the web template

```bash
npm create vite@latest
cd site
npm install
npm run dev
```

2. add a partition `storage,  data, fat, , 2M `

3. configure menuconfig with

- the new partition table
- ensure flash size is correct
- allow FAT long file names

4. in main/CMakeLists add
   `fatfs_create_rawflash_image(storage ../site/dist FLASH_IN_PROJECT)`
