const {app, BrowserWindow} = require('electron') 
const url = require('url') 
const path = require('path')  

let win  

function createWindow() 
{ 
    win = new BrowserWindow( { frame: false, fullscreen: true} )
    win.setPosition( 0, 0 );
    win.setResizable( false );
    win.setFullScreen( true );
   	win.loadURL( url.format ({ 
    	pathname: path.join(__dirname, 'electron.html'), 
      	protocol: 'file:', 
      	slashes: true 
   }))
}  

app.on('ready', createWindow)
