
const registry = new Map;

const diff = (v, dv) => ({ v, dv });
const pure = x => diff(x, new Map);

const named = (x, name) => {
	const m = new Map;
	m.set(name, 1);
	const result = diff(x, m);
	registry.set(name, result);
	return result;
}

const maddimap = (tgt, src, coef) => {
	for (const [k, v] of src.entries()) {
		if (tgt.has(k)) {
			tgt.set(k, tgt.get(k) + v * coef);
		} else {
			tgt.set(k, v * coef);
		}
	}
	return tgt;
};

const scalemap = (x, m) => {
	const result = new Map;
	for (const [k, v] of m.entries()) {
		result.set(k, x * v);
	}
	return result;
};

const addmap = (m1, m2) => maddimap(new Map(m1), m2,  1);
const submap = (m1, m2) => maddimap(new Map(m1), m2, -1);

const add = (x, y) => diff(x.v + y.v, addmap(x.dv, y.dv));
const sub = (x, y) => diff(x.v - y.v, submap(x.dv, y.dv));

const mul = (x, y) => {
	const m = new Map;
	maddimap(m, x.dv, y.v);
	maddimap(m, y.dv, x.v);
	return diff(x.v * y.v, m);
};

const fresh = x => {
	const alpha = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
	let s = "";
	for (let i = 0; i < 10; ++i) {
		s = s + alpha[Math.floor(Math.random() * alpha.length)];
	}
	return named(x, s);
};

const rand = () => Math.random() - 0.5;

const neuron = (weights, bias) => ({ weights, bias });

const layer = neurons => ({ neurons });

const makeLayer = (n, m) => {
    const result = [];
	if (false) {
		const scale = Math.sqrt(2 / m) / 4; // He initialization
		for (let i = 0; i < n; ++i) {
			const weights = [];
			for (let j = 0; j < m; ++j) {
				weights.push(fresh(rand() * scale));
			}
			const bias = fresh(0); // biases often start at 0 for ReLU nets
			result.push(neuron(weights, bias));
		}
	} else {
		for (let i = 0; i < n; ++i) {
			const weights = [];
			for (let j = 0; j < m; ++j) {
				weights.push(fresh(rand() * 0.01));
			}
			const bias = fresh(Math.random() * 0.01);
			result.push(neuron(weights, bias));
		}
	}
	return layer(result);
};

const layerSizes = [2, 8, 8, 1];

const layers = [ ];
for (let i = 1; i < layerSizes.length; ++i) {
	layers.push(makeLayer(layerSizes[i], layerSizes[i-1]));
}

const examples = [
	[[-1, -1], [0]],
	[[-1, +1], [1]],
	[[+1, -1], [1]],
	[[+1, +1], [0]],
];

function eval(input) {
	let data = input.map(x => pure(x));
	for (let i = 1; i < layerSizes.length; ++i) {
		const neurons = layers[i-1].neurons;
		const n = layerSizes[i];
		const m = layerSizes[i-1];
		const output = [];
		for (let j = 0; j < n; ++j) {
			const neuron = neurons[j];
			const weights = neuron.weights;
			const bias = neuron.bias;
			let value = bias;
			for (let k = 0; k < m; ++k) {
				value = add(value, mul(data[k], weights[k]));
			}
			if (value.v < 0) {
				value = pure(0);
			}
			output.push(value);
		}
		data = output;
	}
	return data;
}

const learningRate = 0.1;

for (let iters = 0; iters < 5000; ++iters) {

	let loss = pure(0);

	for (const [input, correct] of examples) {
		const result = eval(input);
		for (let i = 0; i < result.length; ++i) {
			const error = sub(result[i], pure(correct[i]));
			const error2 = mul(error, error);
			loss = add(loss, error2);
		}
	}
	loss = mul(loss, pure(1 / examples.length));

	for (const [k, v] of loss.dv) {
		const node = registry.get(k);
		node.v -= v * learningRate;
	}

	console.log(evalLoss(examples));
}

function evalLoss(examples) {
	let loss = 0;
	for (const [input, correct] of examples) {
		const output = eval(input);
		const clean = output.map(node => node.v);
		for (let i = 0; i < clean.length; ++i) {
			loss += (correct[i] - clean[i])**2;
		}
	}
	loss /= examples.length;
	return Math.sqrt(loss);
}

for (const [input, correct] of examples) {
	const output = eval(input);
	const clean = output.map(node => node.v);
	console.log(input, clean);
}
