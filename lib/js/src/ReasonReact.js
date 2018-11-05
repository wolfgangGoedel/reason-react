'use strict';

var Block = require("bs-platform/lib/js/block.js");
var Curry = require("bs-platform/lib/js/curry.js");
var React = require("react");

function functionComponent(debugName) {
  var functionComponent$1 = function (props) {
    var render = props.render;
    return Curry._1(render, /* () */0);
  };
  functionComponent$1.displayName = debugName;
  return (function (render) {
      return /* Reason */Block.__(0, [
                functionComponent$1,
                render
              ]);
    });
}

function element($staropt$star, $staropt$star$1, component) {
  var key = $staropt$star !== undefined ? $staropt$star : undefined;
  var ref = $staropt$star$1 !== undefined ? $staropt$star$1 : undefined;
  if (component.tag) {
    return Curry._2(component[0], key, ref);
  } else {
    return React.createElement(component[0], {
                key: key,
                ref: ref,
                render: component[1]
              });
  }
}

function useState(initial) {
  var match = React.useState(initial);
  var setValue = match[1];
  return /* tuple */[
          match[0],
          (function (proj) {
              return setValue(Curry.__1(proj));
            })
        ];
}

function useReducer(reducer, initial) {
  var match = React.useReducer(Curry.__2(reducer), initial);
  var dispatch = match[1];
  return /* tuple */[
          match[0],
          (function (action) {
              return dispatch(action);
            })
        ];
}

function wrapJsForReason(reactClass, props, children) {
  var jsElementWrapped = function (param, param$1) {
    var reactClass$1 = reactClass;
    var props$1 = props;
    var children$1 = children;
    var key = param;
    var ref = param$1;
    var props$2 = Object.assign(Object.assign({ }, props$1), {
          ref: ref,
          key: key
        });
    var varargs = /* array */[
        reactClass$1,
        props$2
      ].concat(children$1);
    return React.createElement.apply(null, varargs);
  };
  return /* WrappedJs */Block.__(1, [jsElementWrapped]);
}

function safeMakeEvent(eventName) {
  if (typeof Event === "function") {
    return new Event(eventName);
  } else {
    var $$event = document.createEvent("Event");
    $$event.initEvent(eventName, true, true);
    return $$event;
  }
}

function path(param) {
  var match = typeof (window) === "undefined" ? undefined : (window);
  if (match !== undefined) {
    var raw = match.location.pathname;
    switch (raw) {
      case "" : 
      case "/" : 
          return /* [] */0;
      default:
        var raw$1 = raw.slice(1);
        var match$1 = raw$1[raw$1.length - 1 | 0];
        var raw$2 = match$1 === "/" ? raw$1.slice(0, -1) : raw$1;
        var a = raw$2.split("/");
        var _i = a.length - 1 | 0;
        var _res = /* [] */0;
        while(true) {
          var res = _res;
          var i = _i;
          if (i < 0) {
            return res;
          } else {
            _res = /* :: */[
              a[i],
              res
            ];
            _i = i - 1 | 0;
            continue ;
          }
        };
    }
  } else {
    return /* [] */0;
  }
}

function hash(param) {
  var match = typeof (window) === "undefined" ? undefined : (window);
  if (match !== undefined) {
    var raw = match.location.hash;
    switch (raw) {
      case "" : 
      case "#" : 
          return "";
      default:
        return raw.slice(1);
    }
  } else {
    return "";
  }
}

function search(param) {
  var match = typeof (window) === "undefined" ? undefined : (window);
  if (match !== undefined) {
    var raw = match.location.search;
    switch (raw) {
      case "" : 
      case "?" : 
          return "";
      default:
        return raw.slice(1);
    }
  } else {
    return "";
  }
}

function push(path) {
  var match = typeof (history) === "undefined" ? undefined : (history);
  var match$1 = typeof (window) === "undefined" ? undefined : (window);
  if (match !== undefined && match$1 !== undefined) {
    match.pushState(null, "", path);
    match$1.dispatchEvent(safeMakeEvent("popstate"));
    return /* () */0;
  } else {
    return /* () */0;
  }
}

function url(param) {
  return /* record */[
          /* path */path(/* () */0),
          /* hash */hash(/* () */0),
          /* search */search(/* () */0)
        ];
}

function watchUrl(callback) {
  var match = typeof (window) === "undefined" ? undefined : (window);
  if (match !== undefined) {
    var watcherID = function (param) {
      return Curry._1(callback, url(/* () */0));
    };
    match.addEventListener("popstate", watcherID);
    return watcherID;
  } else {
    return (function (param) {
        return /* () */0;
      });
  }
}

function unwatchUrl(watcherID) {
  var match = typeof (window) === "undefined" ? undefined : (window);
  if (match !== undefined) {
    match.removeEventListener("popstate", watcherID);
    return /* () */0;
  } else {
    return /* () */0;
  }
}

function useEffect(prim, prim$1) {
  React.useEffect(prim, prim$1);
  return /* () */0;
}

var Router = [
  push,
  watchUrl,
  unwatchUrl,
  url
];

exports.functionComponent = functionComponent;
exports.element = element;
exports.useState = useState;
exports.useEffect = useEffect;
exports.useReducer = useReducer;
exports.wrapJsForReason = wrapJsForReason;
exports.Router = Router;
/* react Not a pure module */
