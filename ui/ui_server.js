var express = require('express');
var app = express();


app.use( '/public', express.static('public') );

/*
app.use( '/scripts', express.static(__dirname + '/node_modules/jquery/dist/' ) );
app.use( '/scripts', express.static(__dirname + '/node_modules/bootstrap/dist/' ) );
app.use( '/scripts', express.static(__dirname + '/node_modules/angular' ) );
app.use( '/scripts', express.static(__dirname + '/node_modules/angular-route' ) );
app.use( '/scripts', express.static(__dirname + '/node_modules/angularjs-gauge/dist' ) );
app.use( '/scripts', express.static(__dirname + '/node_modules/angular-material' ) );
app.use( '/scripts', express.static(__dirname + '/node_modules/angular-aria') );
app.use( '/scripts', express.static(__dirname + '/node_modules/angular-sanitize') );
app.use( '/scripts', express.static(__dirname + '/node_modules/angular-animate') );
app.use( '/scripts', express.static(__dirname + '/node_modules/angular-on-screen-keyboard/dist') );
*/
var crontab_dir = '/home/pi/debimeter/ui'
app.use( '/scripts', express.static(crontab_dir + '/node_modules/jquery/dist/' ) );
app.use( '/scripts', express.static(crontab_dir + '/node_modules/bootstrap/dist/' ) );
app.use( '/scripts', express.static(crontab_dir + '/node_modules/angular' ) );
app.use( '/scripts', express.static(crontab_dir + '/node_modules/angular-route' ) );
app.use( '/scripts', express.static(crontab_dir + '/node_modules/angularjs-gauge/dist' ) );
app.use( '/scripts', express.static(crontab_dir + '/node_modules/angular-material' ) );
app.use( '/scripts', express.static(crontab_dir + '/node_modules/angular-aria') );
app.use( '/scripts', express.static(crontab_dir + '/node_modules/angular-sanitize') );
app.use( '/scripts', express.static(crontab_dir + '/node_modules/angular-animate') );
app.use( '/scripts', express.static(crontab_dir + '/node_modules/angular-on-screen-keyboard/dist') );

app.use( '/public', express.static( crontab_dir + '/public'));

app.get('/', function ( req, res ) 
{
   res.sendFile( crontab_dir + "/" + "index.html" );
});

var server = app.listen( 81, function () 
{
   var host = server.address().address
   var port = server.address().port
   
   console.log("Example app listening at http://%s:%s", host, port);
});