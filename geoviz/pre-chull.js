const diagram = document.getElementById('diagram');
const svg = diagram.children[0];

function rnd() {
	return (Math.random() + Math.random() + Math.random() - 1.5) / 3;
}

const ps = Array.from({length: 50}, () => ({ x: rnd()*20, y: rnd()*15 }));
const poly = makePolyline(ps, {fill: 'none', stroke: 'black', 'stroke-width': 0.01});

const circles = ps.map(p => makeCircle(p.x, -p.y, 0.1, {fill: 'red', stroke: 'black', 'stroke-width': 0.05}));

setPoints(poly, ps);

svg.appendChild(poly);
circles.forEach(c => svg.appendChild(c));


function updateSvg() {
	for (let i = 0; i < ps.length; ++i) {
		// setAttributes(circles[i], {cx: ps[i].x, cy: -ps[i].y});
	}
	setPoints(poly, ps.map(p=>({x:p.x, y:-p.y})));
}

function sleep(time) {
	return new Promise((resolve, f2) => {
		setTimeout(resolve, time * 1000)
	});
}

async function sort(arr, l, r, cmp) {
	for (let gap of [23, 10, 4, 1]) {
		for (let i = l; i < r; ++i) {
			for (let j = i; j-gap >= l; --j) {
				if (cmp(arr[j-gap], arr[j]) > 0) {
					let temp = arr[j-gap];
					arr[j-gap] = arr[j];
					arr[j] = temp;
					await sleep(0.05);
					updateSvg();
				} else {
					break;
				}
			}
		}
	}
}

function vsub(a, b) {
	return {x: a.x-b.x, y: a.y-b.y};
}

function cross(a, b) {
	return a.x * b.y - a.y * b.x;
}

function cw(o, a, b) {
	return cross(vsub(a, o), vsub(b, o)) < 0;
}

function ccw(o, a, b) {
	return cross(vsub(a, o), vsub(b, o)) > 0;
}

async function chull() {

	await sort(ps, 0, ps.length, (a, b) => a.x - b.x);

	await sleep(5);

	let i = 2;
	let j = 2;

	while (j < ps.length) {
		while (true) {

			if (i < 2) break;

			const a = ps[i-2];
			const b = ps[i-1];
			const c = ps[j];

			const u = vsub(c, b);
			const v = vsub(a, b);

			const cp = cross(u, v);

			const bad = cp > 0;

			if (bad) {
				ps.splice(i-1, 1);
				i--;
				j--;
				await sleep(3);
				updateSvg();
			} else {
				break;
			}
		}

		ps[i++] = ps[j++];
	}

	ps.length = i;
	await sleep(0.1);
	updateSvg();
}

chull();

