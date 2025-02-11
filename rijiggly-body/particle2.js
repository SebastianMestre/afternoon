
function vec(x, y) { return {x, y, toString() { return `(${this.x}, ${this.y})` }}; }
function copy(a) { return vec(a.x, a.y); }
function addi(a, b) { a.x += b.x; a.y += b.y; return a; }
function subi(a, b) { a.x -= b.x; a.y -= b.y; return a; }
function muli(a, s) { a.x *= s; a.y *= s; return a; }
function add(a, b) { return addi(copy(a), b); }
function sub(a, b) { return subi(copy(a), b); }
function mul(a, s) { return muli(copy(a), s); }
function dot(a, b) { return a.x*b.x + a.y*b.y; }

// normalize vector in-place and return length
function normalizei(a) { const len = Math.sqrt(dot(a,a)); muli(a, 1/len); return len; }

// return normalized vector
function normalize(a) { a = copy(a); normalizei(a); return a; }


const FRAMERATE = 6000;
const DT_IN_SECONDS = 1 / FRAMERATE;

function particle(x, y, mass = 1) {
	return { pos: vec(x,y), vel: vec(0,0), mass };
}

function applyForce(p, f) {
	addi(p.vel, mul(f, DT_IN_SECONDS / p.mass));
}

function spring(p1, p2, length, stiffness) {
	return {p1, p2, length, stiffness};
}

let frame = 0;
function relax(s) {
	const p1 = s.p1;
	const p2 = s.p2;
	const delta = sub(p2.pos, p1.pos);
	const target_length = s.length;
	const actual_length = normalizei(delta);
	const displacement = actual_length - target_length;
	applyForce(p1, mul(delta,  displacement * s.stiffness));
	applyForce(p2, mul(delta, -displacement * s.stiffness));
}

function distance(p1, p2) {
	const delta = sub(p2.pos, p1.pos);
	return Math.sqrt(dot(delta, delta));
}

const springs = [];
const particles = [];

for (let i = 0; i < 11; ++i) {
	for (let j = 0; j < 11; ++j) {
		particles.push(particle(100 + 20 * j, 100 + 20 * i, 4 / (11 * 11)));

		if (i > 0) {
			springs.push(spring(particles[i*11+j], particles[(i-1)*11+j], 20, 100.0));
		}
		if (j > 0) {
			springs.push(spring(particles[i*11+j], particles[i*11+(j-1)], 20, 100.0));
		}
		if (i > 0 && j > 0) {
			springs.push(spring(particles[i*11+j], particles[(i-1)*11+(j-1)], 20 * Math.SQRT2, 100.0));
		}
	}
}
applyForce(particles[0+2],       vec(0,  15 / DT_IN_SECONDS));
applyForce(particles[11*11-1-2], vec(0, -15 / DT_IN_SECONDS));

const canvas = document.getElementById('canvas');
const context = canvas.getContext('2d');

const MAX_REAL_FRAMERATE = 60;

window.setInterval(() => {

	const repetitions = Math.max(1, Math.round(FRAMERATE / MAX_REAL_FRAMERATE));

	for (let iter = 0; iter < repetitions; ++iter) {

		for (let i = 0; i < springs.length; ++i) {
			relax(springs[i]);
		}

		for (let i = 0; i < particles.length; ++i) {
			const p = particles[i];
			addi(p.pos, mul(p.vel, DT_IN_SECONDS));
			muli(p.vel, Math.pow(0.999, DT_IN_SECONDS));
		}

		if (++frame == FRAMERATE) {
			frame = 0;
			let kineticEnergy = 0;
			for (let i = 0; i < particles.length; ++i) {
				const p = particles[i];
				kineticEnergy += p.mass * dot(p.vel, p.vel) / 2;
			}

			let potentialEnergy = 0;
			for (let i = 0; i < springs.length; ++i) {
				const s = springs[i];
				const p1 = s.p1;
				const p2 = s.p2;
				const actual_length = distance(p1, p2);
				const target_length = s.length;
				const displacement = actual_length - target_length;
				potentialEnergy += s.stiffness * displacement**2 / 2;
			}

			console.log("Total energy:", kineticEnergy + potentialEnergy);
		}
	}

	// context.clearRect(0, 0, 800, 450);
	context.fillStyle = 'rgba(255,255,255,0.1)';
	context.fillRect(0, 0, 800, 450);
	context.fillStyle = '#000';
	for (let i = 0; i < particles.length; ++i) {
		const p = particles[i];
		context.fillRect(p.pos.x-2, p.pos.y-2, 4, 4);
	}

	context.beginPath();
	for (let i = 0; i < springs.length; ++i) {
		const p1 = springs[i].p1;
		const p2 = springs[i].p2;
		context.moveTo(p1.pos.x, p1.pos.y);
		context.lineTo(p2.pos.x, p2.pos.y);
	}
	context.stroke();

}, Math.max(DT_IN_SECONDS * 1000, 1000 / MAX_REAL_FRAMERATE));
