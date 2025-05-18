function makeDraggable(state, el) {
    // from https://www.redblobgames.com/making-of/draggable/

    function start(event) {
        if (event.button !== 0) return; // left button only
        let {x, y} = state.eventToCoordinates(event);
        state.dragging = {dx: state.pos.x - x, dy: state.pos.y - y};
        el.setPointerCapture(event.pointerId);
    }

    function end(_event) {
        state.dragging = null;
    }

    function move(event) {
        if (!state.dragging) return;
        let {x, y} = state.eventToCoordinates(event);
        state.pos = {x: x + state.dragging.dx, y: y + state.dragging.dy};
    }

    el.addEventListener('pointerdown', start);
    el.addEventListener('pointerup', end);
    el.addEventListener('pointercancel', end);
    el.addEventListener('pointermove', move)
    el.addEventListener('touchstart', (e) => e.preventDefault());
}

const svgNs = "http://www.w3.org/2000/svg";

function setAttributes(el, attrs) {
	for (const [k, v] of Object.entries(attrs)) {
		el.setAttribute(k, v);
	}
}

function makeRect(x, y, width, height, options = {}) {
	const rect = document.createElementNS(svgNs, 'rect');
	setAttributes(rect, {x, y, width, height, ...options});
	return rect;
}

function makeCircle(cx, cy, r, options = {}) {
	const circle = document.createElementNS(svgNs, 'circle');
	setAttributes(circle, {cx, cy, r, ...options});
	return circle;
}

function makeLine(x1, y1, x2, y2, options = {}) {
	const line = document.createElementNS(svgNs, 'line');
	setAttributes(line, {x1, y1, x2, y2, ...options});
	return line;
}

function setPoints(poly, points) {
	points = points.map(p => [p.x, p.y].join(',')).join(' ');
	setAttributes(poly, {points});
}

function makePolygon(points, options = {}) {
	const polygon = document.createElementNS(svgNs, 'polygon');
	setPoints(polygon, points);
	setAttributes(polygon, options);
	return polygon;
}

function makePolyline(points, options = {}) {
	const polyline = document.createElementNS(svgNs, 'polyline');
	setPoints(polyline, points);
	setAttributes(polyline, options);
	return polyline;
}

function makeText(x, y, textContent, options = {}) {
	const text = document.createElementNS(svgNs, 'text');
	setAttributes(text, {x, y, ...options})
	text.textContent = textContent;
	return text;
}
