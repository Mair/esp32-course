const path = require("path");
const fs = require("fs");
const { execSync } = require("child_process");
const exec = require('child_process').exec;

const res = execSync("powershell.exe c:\\esp\\esp-idf\\export.ps1")
console.log(res.toString())

const getAllIDFProj = (dirPath)=> {
    if(dirPath == path.join(__dirname,".git") ||
    dirPath == path.join(__dirname,".vscode") ||
    dirPath == path.join(__dirname,".vscode") ||
    dirPath.includes("\\build")
    ){
        return;
    }
   
    const files_dirs = fs.readdirSync(dirPath)
    const is_idf = files_dirs.find(files_dir=> files_dir == "main" && 
        fs.statSync(path.join( dirPath, files_dir)).isDirectory()) &&
        files_dirs.includes("CMakeLists.txt")
    if(is_idf) {
        try{
            console.log("attempting to build ", dirPath);
            execSync(`cd ${dirPath} && idf.py fullclean && idf.py build`)
            console.log("✅", dirPath, "succeeded")
            console.log("")
        } catch(err){
            console.log("❌ Error compiling ",dirPath)
            //console.error(err);
            console.log("")
        }
    }
    files_dirs.forEach(files_dir=>{
        const child_path = path.join( dirPath, files_dir)
        if (fs.statSync(child_path).isDirectory()) {
            arrayOfFiles = getAllIDFProj(child_path)
        }
    })
  }


  getAllIDFProj(__dirname);