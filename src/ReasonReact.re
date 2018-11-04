type reactClass;

/* type jsProps; */

type reactElement;

type reactRef;

[@bs.val] external null : reactElement = "null";

external string : string => reactElement = "%identity";

external array : array(reactElement) => reactElement = "%identity";

external refToJsObj : reactRef => Js.t({..}) = "%identity";

[@bs.splice] [@bs.val] [@bs.module "react"]
external createElement :
  (reactClass, ~props: Js.t({..})=?, array(reactElement)) => reactElement =
  "createElement";

[@bs.splice] [@bs.module "react"]
external cloneElement :
  (reactElement, ~props: Js.t({..})=?, array(reactElement)) => reactElement =
  "cloneElement";

[@bs.val] [@bs.module "react"]
external createElementVerbatim : 'a = "createElement";

type renderClosure = unit => reactElement;

type componentFunction = {. "render": renderClosure} => reactElement;

type component =
  | Reason(componentFunction, renderClosure)
  | WrappedJs(jsElementWrapped)

/* and jsPropsToReason('jsProps) =
  'jsProps => component */
/***
 * Type of hidden field for Reason components that use JS components
 */
and jsElementWrapped =
  (
    ~key: Js.nullable(string),
    ~ref: Js.nullable(Js.nullable(reactRef) => unit)
  ) =>
  reactElement;
  
let functionComponent: (string, renderClosure) => component =
  debugName => {
    let functionComponent = props => {
      let render = props##render;
      render();
    };
    Obj.magic(functionComponent)##displayName #= debugName;
    render => Reason(functionComponent, render);
  };

/***
 * Convenience for creating React elements before we have a better JSX transform.  Hopefully this makes it
 * usable to build some components while waiting to migrate the JSX transform to the next API.
 *
 * Constrain the component here instead of relying on the Element constructor which would lead to confusing
 * error messages.
 */
let element =
    (
      ~key: string=Obj.magic(Js.Nullable.undefined),
      ~ref: Js.nullable(reactRef) => unit=Obj.magic(Js.Nullable.undefined),
      component: component,
    ) => {
  switch (component) {
  | WrappedJs(jsElementWrapped) =>
    jsElementWrapped(
      ~key=Js.Nullable.return(key),
      ~ref=Js.Nullable.return(ref),
    )
  | Reason(component, render) =>
    createElement(
      Obj.magic(component),
      ~props={"key": key, "ref": ref, "render": render},
      [||],
    )
  };
};

/* let wrapReasonForJs =
    (
      ~component,
      jsPropsToReason:
        jsPropsToReason('jsProps),
    ) => {
  let jsPropsToReason:
    jsPropsToReason(jsProps) =
    Obj.magic(jsPropsToReason) /* cast 'jsProps to jsProps */;
  Obj.magic(component.reactClassInternal)##prototype##jsPropsToReason#=(
                                                                    Some(
                                                                    jsPropsToReason,
                                                                    )
                                                                    );
  component.reactClassInternal;
}; */

module WrapProps = {
  /* We wrap the props for reason->reason components, as a marker that "these props were passed from another
     reason component" */
  let wrapProps =
      (
        ~reactClass,
        ~props,
        children,
        ~key: Js.nullable(string),
        ~ref: Js.nullable(Js.nullable(reactRef) => unit),
      ) => {
    let props =
      Js.Obj.assign(
        Js.Obj.assign(Js.Obj.empty(), Obj.magic(props)),
        {"ref": ref, "key": key},
      );
    let varargs =
      [|Obj.magic(reactClass), Obj.magic(props)|]
      |> Js.Array.concat(Obj.magic(children));
    /* Use varargs under the hood */
    Obj.magic(createElementVerbatim)##apply(Js.Nullable.null, varargs);
  };
  let wrapJsForReason =
      (~reactClass, ~props, children)
      : component => {
    let jsElementWrapped = wrapProps(~reactClass, ~props, children);
    WrappedJs(jsElementWrapped);
  };
};

let wrapJsForReason = WrapProps.wrapJsForReason;

[@bs.module "react"] external fragment : 'a = "Fragment";

module Router = {
  [@bs.get] external location : Dom.window => Dom.location = "";

  [@bs.send]
  /* actually the cb is Dom.event => unit, but let's restrict the access for now */
  external addEventListener : (Dom.window, string, unit => unit) => unit = "";

  [@bs.send]
  external removeEventListener : (Dom.window, string, unit => unit) => unit =
    "";

  [@bs.send] external dispatchEvent : (Dom.window, Dom.event) => unit = "";

  [@bs.get] external pathname : Dom.location => string = "";

  [@bs.get] external hash : Dom.location => string = "";

  [@bs.get] external search : Dom.location => string = "";

  [@bs.send]
  external pushState :
    (Dom.history, [@bs.as {json|null|json}] _, [@bs.as ""] _, ~href: string) =>
    unit =
    "";

  [@bs.val] external event : 'a = "Event";

  [@bs.new] external makeEventIE11Compatible : string => Dom.event = "Event";

  [@bs.val] [@bs.scope "document"]
  external createEventNonIEBrowsers : string => Dom.event = "createEvent";

  [@bs.send]
  external initEventNonIEBrowsers : (Dom.event, string, bool, bool) => unit =
    "initEvent";

  let safeMakeEvent = eventName =>
    if (Js.typeof(event) == "function") {
      makeEventIE11Compatible(eventName);
    } else {
      let event = createEventNonIEBrowsers("Event");
      initEventNonIEBrowsers(event, eventName, true, true);
      event;
    };

  /* This is copied from array.ml. We want to cut dependencies for ReasonReact so
     that it's friendlier to use in size-constrained codebases */
  let arrayToList = a => {
    let rec tolist = (i, res) =>
      if (i < 0) {
        res;
      } else {
        tolist(i - 1, [Array.unsafe_get(a, i), ...res]);
      };
    tolist(Array.length(a) - 1, []);
  };
  /* if we ever roll our own parser in the future, make sure you test all url combinations
     e.g. foo.com/?#bar
     */
  /* sigh URLSearchParams doesn't work on IE11, edge16, etc. */
  /* actually you know what, not gonna provide search for now. It's a mess.
     We'll let users roll their own solution/data structure for now */
  let path = () =>
    switch ([%external window]) {
    | None => []
    | Some((window: Dom.window)) =>
      switch (window |> location |> pathname) {
      | ""
      | "/" => []
      | raw =>
        /* remove the preceeding /, which every pathname seems to have */
        let raw = Js.String.sliceToEnd(~from=1, raw);
        /* remove the trailing /, which some pathnames might have. Ugh */
        let raw =
          switch (Js.String.get(raw, Js.String.length(raw) - 1)) {
          | "/" => Js.String.slice(~from=0, ~to_=-1, raw)
          | _ => raw
          };
        raw |> Js.String.split("/") |> arrayToList;
      }
    };
  let hash = () =>
    switch ([%external window]) {
    | None => ""
    | Some((window: Dom.window)) =>
      switch (window |> location |> hash) {
      | ""
      | "#" => ""
      | raw =>
        /* remove the preceeding #, which every hash seems to have.
           Why is this even included in location.hash?? */
        raw |> Js.String.sliceToEnd(~from=1)
      }
    };
  let search = () =>
    switch ([%external window]) {
    | None => ""
    | Some((window: Dom.window)) =>
      switch (window |> location |> search) {
      | ""
      | "?" => ""
      | raw =>
        /* remove the preceeding ?, which every search seems to have. */
        raw |> Js.String.sliceToEnd(~from=1)
      }
    };
  let push = path =>
    switch ([%external history], [%external window]) {
    | (None, _)
    | (_, None) => ()
    | (Some((history: Dom.history)), Some((window: Dom.window))) =>
      pushState(history, ~href=path);
      dispatchEvent(window, safeMakeEvent("popstate"));
    };
  type url = {
    path: list(string),
    hash: string,
    search: string,
  };
  type watcherID = unit => unit;
  let url = () => {path: path(), hash: hash(), search: search()};
  /* alias exposed publicly */
  let dangerouslyGetInitialUrl = url;
  let watchUrl = callback =>
    switch ([%external window]) {
    | None => (() => ())
    | Some((window: Dom.window)) =>
      let watcherID = () => callback(url());
      addEventListener(window, "popstate", watcherID);
      watcherID;
    };
  let unwatchUrl = watcherID =>
    switch ([%external window]) {
    | None => ()
    | Some((window: Dom.window)) =>
      removeEventListener(window, "popstate", watcherID)
    };
};
