/**
 * app.js
 *   Main app for home control.
 */

require(["dojo/dom",
         "dojo/on",
         "dojo/request",
         "dojo/parser",
         "dojo/aspect",
         "dijit/registry",
         "dojox/mobile",
         "dojox/mobile/compat",
         "dojox/mobile/ListItem",
         "dojox/mobile/Switch",
         "dojox/mobile/EdgeToEdgeList",
         "dojox/mobile/View",
         "dojox/mobile/Heading",
         "dojox/mobile/Pane",
         "dojo/ready"],
  function(dom, on,request, parser, aspect, registry,
           mobile, compat, ListItem, Switch) {
    parser.parse().then(function() {
      var arr,
          workspace,
          values = [ ],
          parent = registry.byId('edgeList');

      request.get( "/init.json", { handleAs:"json" } ).then( function(res) {
        arr = res;
        arr.forEach(function(v, i) {
          var iconIdx = i%3;
          values[v.idx] = v.val === 'true' ? 1 : 0;
          item = new ListItem( { iconPos: "0,"+(iconIdx*29)+",29,29",
                                 switchIdx: v.idx,
                                 label: v.name } );
          item.addChild( new Switch( { class: "mblItemSwitch",
                                       value: v.val === 'true' ? 'on' : 'off',
                                       onStateChanged: function( val ) {
                                         values[v.idx] = (val === 'on' ? 1 : 0);
                                         var postMsg = 'switch';
                                         values.forEach(function(i) { postMsg += i; });
                                         request.post( postMsg );
                                      }}));
          parent.addChild(item);
        });
      });

      // setup + button
      on( dom.byId('idAddOutlet'), 'click', function() {
        request.post( 'plusOutlet' );
      });

      // setup blockly
      var blocklyArea = dom.byId('blocklyArea'),
          blocklyDiv = dom.byId('blocklyDiv'),
          onresize;

      workspace = Blockly.inject(blocklyDiv, {toolbox:  '<xml>' +
                                                        '<block type="controls_if"></block>' +
                                                        '<block type="controls_repeat_ext"></block>' +
                                                        '<block type="logic_compare"></block>' +
                                                        '</xml>'});
      onresize = function(e) {
        var x=0, y=0, element = blocklyArea;

        element.style.width = '100%';
        element.style.height = '100%';

        do {
          x += element.offsetLeft;
          y += element.offsetTop;
          element = element.offsetParent;
        } while(element);

        blocklyDiv.style.left = x + 'px';
        blocklyDiv.style.top = y + 'px';
        blocklyDiv.style.width = blocklyArea.offsetWidth + 'px';
        blocklyDiv.style.height = blocklyArea.offsetHeight + 'px';
      };

      aspect.after( registry.byId('view3'), "onAfterTransitionIn", function() {
        onresize();
        Blockly.fireUiEvent( window, 'resize' );
      });

      window.addEventListener('resize', onresize, false);
      onresize();
    }); // parser
  }); // main function
