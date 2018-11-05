/***
 * This API assumes that JSX will desugar <Foo key ref attr1=val1 attrn=valn /> into:
 *
 * ReasonReact.element(
 *   Foo.make(~key, ~ref, ~attr1=val1, ~attrn=valn, [| |]
 * )
 */

type reactClass;

type reactElement;

type reactRef;

[@bs.val] external null : reactElement = "null";

external string : string => reactElement = "%identity";

external array : array(reactElement) => reactElement = "%identity";

external refToJsObj : reactRef => Js.t({..}) = "%identity";

/* This should _not_ be used directly, unless you're passing a class like this:

switch (actionsClass) {
| Some(actions) =>
    ReasonReact.createElement(
      actions,
      ~props={
        "className": "hi"
      },
      [|whatever|],
    )
}

In every other case, you should be using the JSX
*/
[@bs.splice] [@bs.val] [@bs.module "react"]
external createElement :
  (reactClass, ~props: Js.t({..})=?, array(reactElement)) => reactElement =
  "createElement";

[@bs.splice] [@bs.module "react"]
external cloneElement :
  (reactElement, ~props: Js.t({..})=?, array(reactElement)) => reactElement =
  "cloneElement";

type renderClosure = unit => reactElement;

type componentFunction = {. "render": renderClosure} => reactElement;

/*** For internal use only */
type jsElementWrapped;

type component =
  | Reason(componentFunction, renderClosure)
  | WrappedJs(jsElementWrapped)

let functionComponent: (string, renderClosure) => component;

let element:
  (
    ~key: string=?,
    ~ref: Js.nullable(reactRef) => unit=?,
    component
  ) =>
  reactElement;

/* type jsPropsToReason('jsProps) =
  'jsProps => component; */

/* Hooks */

let useState: 'a => ('a, ('a => 'a) => unit);

let useEffect: (unit => option(unit => unit), 'a) => unit;

let useReducer:
  (('state, 'action) => 'state, 'state) => ('state, 'action => unit);


/***
 * We *under* constrain the kind of component spec this accepts because we actually extend the *originally*
 * defined component. It uses mutation on the original component, so that even if it is extended with
 * {...component}, all extensions will also see the underlying js class. I can sleep at night because js
 * interop is integrating with untyped, code and it is *possible* to create pure-ReasonReact apps without JS
 * interop entirely. */
/* let wrapReasonForJs:
  (
    ~component: component,
    jsPropsToReason(_)
  ) =>
  reactClass; */

/**
 * Wrap props into a JS component
 * Use for interop when Reason components use JS components
 */
let wrapJsForReason:
  (~reactClass: reactClass, ~props: 'a, 'b) =>
  component;

module Router: {
  /** update the url with the string path. Example: `push("/book/1")`, `push("/books#title")` */
  let push: string => unit;
  type watcherID;
  type url = {
    /* path takes window.location.path, like "/book/title/edit" and turns it into `["book", "title", "edit"]` */
    path: list(string),
    /* the url's hash, if any. The # symbol is stripped out for you */
    hash: string,
    /* the url's query params, if any. The ? symbol is stripped out for you */
    search: string,
  };
  /** start watching for URL changes. Returns a subscription token. Upon url change, calls the callback and passes it the url record */
  let watchUrl: (url => unit) => watcherID;
  /** stop watching for URL changes */
  let unwatchUrl: watcherID => unit;
  /** this is marked as "dangerous" because you technically shouldn't be accessing the URL outside of watchUrl's callback;
      you'd read a potentially stale url, instead of the fresh one inside watchUrl.

      But this helper is sometimes needed, if you'd like to initialize a page whose display/state depends on the URL,
      instead of reading from it in watchUrl's callback, which you'd probably have put inside didMount (aka too late,
      the page's already rendered).

      So, the correct (and idiomatic) usage of this helper is to only use it in a component that's also subscribed to
      watchUrl. Please see https://github.com/reasonml-community/reason-react-example/blob/master/src/todomvc/TodoItem.re
      for an example.
      */
  let dangerouslyGetInitialUrl: unit => url;
};

[@bs.module "react"] external fragment: 'a = "Fragment";
