app.controller("measurement1Ctrl", function ( $scope, socket ) 
{
    $scope.msg = "Ölçüm Ekranı 2";

    var vm = this;
    vm.selected = 
    {
        title : 'Debi',
        value: 0,
        unit : 'm3/saat',
        min : "0",
        max : "500000",
        foregroundColor: 'rgba(0, 150, 136, 1)',
        backgroundColor: 'rgba(0, 0, 0, 0.1)',
        id : 14,
        digit: 0
    };

    vm.thresholds = {
      '0': {color: 'green'},
      '8': {color: "orange"},
      '20': {color: 'yellow'},
      '30': {color: 'coral'},
      '80': {color: 'red'}
    }
    vm.empty = {};

    $scope.channels = [
    {   // 14
        title:"Debi",
        value:1,
        unit:"m3/saat",
        min:"0",
        max:"500000",
        digit: 0,
        id : 14
    },
    {   // 2
        title:"Hız (AVG)",
        value:0,
        unit:"m/s",
        min:"0.000",
        max:"10.000",
        digit: 3,
        id : 2
    },
    {   // 19
        title:"Seviye",
        value:0,
        unit:"mm",
        min:"0",
        max:"8000",
        digit: 0,
        id : 19
    },
    {   // 0
        title:"Hız_QP",
        value: 0.0,
        unit:"m/s",
        min: "0",
        max: "100",
        digit: 3,
        id : 0
    },
    {   // 1
        title:"Hız_QPF",
        value:0,
        unit:"m/s",
        min:"0",
        max:"100",
        digit: 3,
        id : 1
    },
    {   // 3
        title:"Hız_RAW",
        value:0,
        unit:"m/s",    
        min:"0",
        max:"100",
        digit: 3,
        id : 3
    },
    {   // 4
        title:"VSN",
        value:0,
        unit:"--",
        min:"0",
        max:"100",
        digit: 3,
        id : 4
    },
    {   // 5
        title:"SNR",
        value:0,
        unit:"--",
        minimum:"0",
        max:"100",
        digit: 3,
        id : 5
    },
    {   // 6
        title:"AGC",
        value:0,
        unit:"--",
        min:"0",
        max:"100",
        digit: 3,
        id : 6
    },
    {   // 7
        title:"NOT",
        value:0,
        unit:"--",
        min:"0",
        max:"100",
        digit: 0,
        id : 7
    },
    {   // 8
        title:"Sıcaklık",
        value: 0.0,
        unit:"*C",
        min:"0",
        max:"60",
        digit: 1,
        id : 8
    },
    {   // 9
        title:"Nem",
        value:0,
        unit:"%",
        min:"0",
        max:"100",
        digit: 2,
        id : 9
    },
    {   // 10
        title:"Basınç",
        value:0,
        unit:"Bar",
        min:"0.00",
        max:"2.00",
        digit: 2,
        id : 10
    },
    {   // 11
        title:"St. Sapma",
        value:0,
        unit:"--",
        min:"0",
        max:"100",
        digit: 1,
        id : 11
    },
    {   // 12
        title:"Genlik",
        value:0,
        unit:"( A ) ",
        min:"0",
        max:"100",
        digit: 0,
        id : 12
    },
    {   // 13
        title:"Level Output",
        value:0,
        unit:"--",
        min:"0",
        max:"100",
        digit: 0,
        id : 13
    },
    {   // 15
        title:"Level Input",
        value:0,
        unit:"--",
        min:"0",
        max:"100",
        digit: 0,
        id : 15
    },
    {   // 16
        title:"AD Voltaj",
        value:0,
        unit:"mV",
        min:"200",
        max:"2500",
        digit: 1,
        id:16
    },
    {   // 17
        title:"AD Akım",
        value:0,
        unit:"mA",
        min:"3",
        max:"25",
        digit: 2,
        id : 17
    },
    {   // 18
        title:"AD Değeri",
        value:0,
        unit:"--",
        min:"0",
        max:"50000",
        digit: 0,
        id :18
    }];


    $scope.channelRowClick = function( rowIndex )
    {
        vm.selected.title = $scope.channels[ rowIndex ].title;
        vm.selected.value = $scope.channels[ rowIndex ].value;
        vm.selected.unit = $scope.channels[ rowIndex ].unit;
        vm.selected.digit = $scope.channels[ rowIndex ].digit;
        vm.selected.min = $scope.channels[ rowIndex ].min;
        vm.selected.max = $scope.channels[ rowIndex ].max;
        vm.selected.id = rowIndex;
    }

    socket.onmessage( function( event ) 
    {
        //console.log( event.data );
        var data = JSON.parse(event.data);
        
        if( data.messageId != 2 )
        {
            return;
        }
        //console.log(data);
        for( var k = 0; k < data.data.length ; k++)
        {
            var dataObj = data.data[k];
            if( dataObj.id == 14 )
            {
                dataObj.value = (dataObj.value / 24).toFixed(3); // m3/gun to m3/saat
            }
            
            for( var m = 0; m < $scope.channels.length; m++)
            {
                if( $scope.channels[m].id == dataObj.id )
                {
                    $scope.channels[m].value = dataObj.value;
                }
            }
            if( vm.selected.id == dataObj.id )
            {
                vm.selected.value = dataObj.value;
            }
        };
        $scope.$apply(); // You shouldn't ever need to call $apply unless you are interfacing from a non-Angular event. 
        //$scope.testVar = 
    });
});