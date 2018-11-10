var app = angular.module("myApp", [ 'ngRoute', 'angularjs-gauge', 'ngMaterial', 'ngAnimate' ] );

app.config(function( $routeProvider ) 
{
    $routeProvider
    .when("/", {
        templateUrl : "public/pages/measurement2.html",
		controller : "measurement2Ctrl"
    })
    .when("/calibration", {
        templateUrl : "public/pages/calibration.html",
        controller : "calibrationCtrl"
    })
    .when("/settings", {
        templateUrl : "public/pages/settings.html",
        controller : "settingsCtrl"
    })
    .when("/diagnostics", {
        templateUrl : "public/pages/diagnostics.html",
        controller: "diagnosticsCtrl"
    })
    .when("/about", {
        templateUrl : "public/pages/about.html",
        controller : "aboutCtrl"
    })
    .when("/m1", {
        templateUrl : "public/pages/main.html",
        controller : "mainCtrl"
    })
    .when("/m2", {
        templateUrl : "public/pages/measurement1.html",
        controller : "measurement1Ctrl"
    })
    .when("/m3", {
        templateUrl : "public/pages/measurement3.html",
        controller : "measurement3Ctrl"
    });
});