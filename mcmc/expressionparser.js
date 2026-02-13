function parseExpression(formula) {
    let cursor = 0;

    function skipWhitespace() {
        while (peek() === ' ' || peek() === '\t' || peek() === '\n') {
            cursor++;
        }
    }

    function match(char) {
        if (peek() === char) {
            return true;
        }
        return false;
    }

    function peek() {
        return cursor < formula.length ? formula[cursor] : null;
    }

    function consume(char) {
        if (match(char)) {
            cursor++;
            return true;
        }
        return false;
    }

    const digits = "0123456789";
    const alpha = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

    function pTerm() {
        skipWhitespace();
        if (consume('(')) {
            const expr = pExpr();
            skipWhitespace();
            if (consume(')')) {
                return expr;
            }
            fail('Expected )');
        } else if (digits.includes(peek())) {
            return parseNumber();
        } else if (alpha.includes(peek())) {
            return parseVariable();
        } else {
            fail('Expected number, variable, or (');
        }
    }

    function parseNumber() {
        let number = '';
        while (digits.includes(peek())) {
            number += peek();
            cursor++;
        }
        if (consume('.')) {
            number += '.';
            while (digits.includes(peek())) {
                number += peek();
                cursor++;
            }
        }
        return Number(number);
    }

    function parseVariable() {
        let variable = '';
        while (alpha.includes(peek())) {
            variable += peek();
            cursor++;
        }
        return variable;
    }

    function getPrecedence(op) {
        if (op === '+') return [10, 11];
        if (op === '-') return [10, 11];
        if (op === '*') return [20, 21];
        if (op === '/') return [20, 21];
        if (op === '^') return [31, 30]; // NOTE: x^y^z is parsed as x^(y^z)
        throw new Error(`Unknown operator: '${op}'`);
    }

    function pExpr(prec = 0) {
        let lhs = pTerm();

        while (true) {
            skipWhitespace();
            const op = peek();

            if (op === null || op === ')') {
                break;
            }

            if (op == '(') {
                if (typeof lhs !== 'string' && typeof lhs !== 'array') {
                    fail('Cannot call operand as a function');
                }
                cursor++;
                const args = [];
                while (true) {
                    skipWhitespace();
                    if (consume(')')) {
                        break;
                    }
                    args.push(pExpr());
                    skipWhitespace();
                    if (consume(',')) {
                        continue;
                    }
                }
                lhs = [lhs, ...args];
                continue;
            }

            if (!"+-*/^".includes(op)) {
                fail(`Expected operator but got '${op}'`);
            }

            const [lprec, rprec] = getPrecedence(op);

            if (lprec < prec) {
                break;
            }

            cursor++;

            const rhs = pExpr(rprec);
            lhs = [op, lhs, rhs];
        }

        return lhs;
    }

    function fail(message) {
        const l = Math.max(0, cursor - 10);
        const r = Math.min(formula.length, cursor + 10);
        const context = formula.slice(Math.max(0, cursor - 10), cursor + 10);
        const arrow = ' '.repeat(cursor - l) + '^';
        throw new Error(`${message} at position ${cursor}
${context}
${arrow}`);
    }

    return pExpr();
}