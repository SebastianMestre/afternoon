

/*

type Mark = "O" | "X";
type Cell = Mark | "_";

interface Board {
	mark(number, number, Mark): Board;
	winner(): Cell;
	toString(): string;
}

*/

class Board {
	constructor(data) {
		this.data = data;
	}

	static empty() {
		return new Board(new Array(9).fill("_"));
	}

	mark(row, col, mark) {
		const result = this.data.slice();
		const index = this.#index(row, col);
		if (result[index] !== "_") {
			console.log(this);
			console.log(row, col);
			throw new Error("overwritting already marked position");
		}
		result[index] = mark;
		return new Board(result);
	}

	winner() {
		const configurations = [
			// horizontal
			[[0, 0], [0, 1], [0, 2]],
			[[1, 0], [1, 1], [1, 2]],
			[[2, 0], [2, 1], [2, 2]],
			// vertical
			[[0, 0], [1, 0], [2, 0]],
			[[0, 1], [1, 1], [2, 1]],
			[[0, 2], [1, 2], [2, 2]],
			// diagonal
			[[0, 0], [1, 1], [2, 2]],
			[[0, 2], [1, 1], [2, 0]],
		];

		for (const configuration of configurations) {
			const value = this.#valueAt(...configuration[0]);

			if (value == "_")
				continue;

			if (configuration.every(pos => this.#valueAt(...pos) == value))
				return value;

		}

		return "_";
	}

	toString() {
		return this.data.join("");
	}
	
	isFull() {
		return !this.data.includes("_");
	}

	#valueAt(row, col) {
		const index = this.#index(row, col);
		return this.data[index];
	}

	#index(row, col) {
		return row * 3 + col;
	}
}

/*

interface Strategy {
	play() : [number, number];
	favor(number, number) : Strategy;
	avoid(number, number) : Strategy;
}

*/

class Strategy {
	constructor(preferences) {
		this.preferences = preferences;
	}

	static defaultStrategy(board) {
		// TODO: clean this up
		return new Strategy(board.data.map(c => c == "_" ? 10 : null));
		// return new Strategy(new Array(9).fill(10));
	}

	play() {
		const sum = this.preferences.reduce((a, b) => a + b);
		const randomSelection = Math.random() * sum;

		for (let current = randomSelection, i = 0; i < this.preferences.length; ++i) {
			if (this.preferences[i] === null)
				continue;

			if (current <= this.preferences[i])
				return this.#rowcol(i);

			current -= this.preferences[i];
		}

		console.log(randomSelection);
		console.log(this.preferences);
		throw new Error("Reached end of selection");
	}

	favor(row, col) {
		const result = new Strategy(this.preferences.slice());
		result.#favor(row, col);
		return result;
	}

	avoid(row, col) {
		const result = new Strategy(this.preferences.slice());
		result.#avoid(row, col);
		return result;
	}

	#favor(row, col) {
		const index = this.#index(row, col);
		this.preferences[index]+=3;
	}

	#avoid(row, col) {
		const index = this.#index(row, col);
		if (this.preferences[index] > 0)
			this.preferences[index]--;
	}

	#index(row, col) {
		return row * 3 + col;
	}

	#rowcol(i) {
		return [(i / 3) | 0, i % 3];
	}
}

/*

interface Player {
	takeMove(Board) : [number, number];
}

interface Learner {
	winningGame(Array<[number, number, Board]>) : void;
	losingGame(Array<[number, number, Board]>) : void;
}

*/

class Menace {
	constructor() {
		this.strategies = new Map();
	}

	takeMove(board) {
		const strategy = this.#getStrategy(board);
		return strategy.play();
	}

	winningGame(moves) {
		for(const move of moves)
			this.#goodMove(...move);
	}

	losingGame(moves) {
		for (const move of moves)
			this.#badMove(...move);
	}

	#goodMove(row, col, board) {
		this.#updateStrategy(board, strategy => strategy.favor(row, col));
	}

	#badMove(row, col, board) {
		this.#updateStrategy(board, strategy => strategy.avoid(row, col));
	}

	#updateStrategy(board, update) {
		const oldStrategy = this.#getStrategy(board);
		const newStrategy = update(oldStrategy);
		this.#setStrategy(board, newStrategy);
	}

	#getStrategy(board) {
		const boardId = board.toString();
		this.#populateStrategyIfMissing(board);
		return this.strategies.get(boardId);
	}

	#setStrategy(board, newStrategy) {
		const boardId = board.toString();
		this.strategies.set(boardId, newStrategy);
	}

	#populateStrategyIfMissing(board) {
		const boardId = board.toString();
		if (!this.strategies.has(boardId))
			this.strategies.set(boardId, Strategy.defaultStrategy(board));
	}
}

class UserPlayer {
	takeMove(board) {
		const str = board.toString();
		const s1 = str.slice(0, 3);
		const s2 = str.slice(3, 6);
		const s3 = str.slice(6, 9);
		const retval = prompt("\n"+s1+"\n"+s2+"\n"+s3);
		const index = Number(retval);
		return [index/3|0, index%3];
	}

	winningGame(moves) {
	}

	losingGame(moves) {
	}

}

let realPerson = false;
function run(player1, player2) {
	const p1Moves = [];
	const p2Moves = [];

	let board = Board.empty();

	const p1Data = ["X", player1, p1Moves];
	const p2Data = ["O", player2, p2Moves];

	while (true) {
		if (step(p1Data, p2Data) || board.isFull()) {
			if (realPerson) {
				drawBoard();
				if (board.winner() !== "_") {
					alert("P1 wins");
				} else {
					alert("EMPATE")
				}
			}
			break;
		}

		if (step(p2Data, p1Data) || board.isFull()) {
			if (realPerson) {
				drawBoard();
				if (board.winner() !== "_") {
					alert("P2 wins");
				} else {
					alert("EMPATE")
				}
			}
			break;
		}
	}

	function drawBoard() {
		const str = board.toString();
		const s1 = str.slice(0, 3);
		const s2 = str.slice(3, 6);
		const s3 = str.slice(6, 9);
		console.log("\n"+s1+"\n"+s2+"\n"+s3);
	}

	// playing: [Mark, Player, Moves]
	// waiting: [Mark, Player, Moves]
	function step(playing, waiting) {
		const [mark, player, moves] = playing;

		const pos = player.takeMove(board);

		moves.push([...pos, board]);

		board = board.mark(...pos, mark);

		if (board.winner() === mark) {
			const [_, loser, loserMoves] = waiting;

			player.winningGame(moves);
			loser.losingGame(loserMoves);

			return true;
		}

		return false;
	}

}

const player = new Menace();

window.player = player;

console.log("!!!!!");

for (let i = 10000; i-- > 0;)
	run(player, player);

const user = new UserPlayer();

realPerson = true;
while (true)
	run(player, user);

