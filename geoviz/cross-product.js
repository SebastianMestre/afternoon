const diagram = document.getElementById('diagram');
const svg = diagram.children[0];

const p1 = {x: 4, y: 6};
const p2 = {x: 5, y: -8};

const labelStyle = {'font-size': 1, fill: 'black', stroke: 'white', 'stroke-width': 0.3, 'paint-order': 'stroke'};

const poly = makePolygon([]);
const text1 = makeText(0, 0, '', labelStyle);

const labelA = makeText(0, 0, 'A', labelStyle);
const labelB = makeText(0, 0, 'B', labelStyle);

updatePoly();

svg.appendChild(poly)
makeDraggableVector(p1);
makeDraggableVector(p2);
svg.appendChild(text1);
svg.appendChild(labelA);
svg.appendChild(labelB);

function updatePoly() {
	// SVG coordinate system has y pointing down, so we just flip this sign to
	// make thigs visually correct.
	const area = -(p1.x * p2.y - p1.y * p2.x);
	const color = area < 0 ? 'rgba(255,90,90,0.5)' : 'rgba(90,90,255,0.5)';
	setPoints(poly, [
		{x: 0, y: 0},
		p1,
		{x: p1.x+p2.x, y: p1.y+p2.y},
		p2,
	]);
	setAttributes(poly, {fill: color});

	text1.textContent = area.toFixed(2);
	setAttributes(text1, {x: (p1.x+p2.x)/2, y: (p1.y+p2.y)/2});

	setAttributes(labelA, {x: p1.x+1.1, y: p1.y});
	setAttributes(labelB, {x: p2.x+1.1, y: p2.y});
}

function makeDraggableVector(p) {
	const circle = makeCircle(p.x, p.y, 0.5, {fill: 'red', stroke: 'black', 'stroke-width': 0.2});
	const line = makeLine(0, 00, p.x, p.y, {stroke: 'black', 'stroke-width': 0.2});
	svg.appendChild(line);
	svg.appendChild(circle);
	makeDraggable({
		eventToCoordinates(event) {
			const svg = event.currentTarget.ownerSVGElement;
			let p = svg.createSVGPoint();
			p.x = event.clientX;
			p.y = event.clientY;
			p = p.matrixTransform(svg.getScreenCTM().inverse());
			return p;
		},
		get pos() {
			return p;
		},
		set pos(newPos) {
			const { x, y } = newPos;
			p.x = x;
			p.y = y;
			setAttributes(circle, {cx: x, cy: y});
			setAttributes(line, {x2: x, y2: y});
			updatePoly();
		}}, circle);
}

