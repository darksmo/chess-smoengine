chess-smoengine
===============

Simple Chess Engine

Still in development during my free time, a chess engine with a simple
interface that can be easily integrated in your own C chess program.

Features currently implemented:
- tests for bitboards, placement of pieces, legal moves

- create bitboards out of your own representation of a chessboard

- more complete structure for a test of legal moves, which checks if moves from
  real games are considered legal

- not yet optimised bitboard infrastructure to represent a chessboard:

- search algorithm
 * negamax + alpha/beta pruning

- special moves:
 * en passant capture
 * castling

- evaluation function

- moves
 * pawns movements/attacks
 * kings movements/attacks
 * knight movements/attacks
 * rook movements/attacks
 * bishop movements/attacks
 * queen movements/attacks

Plan:

- improve chessboard querying

- finish up the test for legal moves

- special moves:
 * promotion


[![githalytics.com alpha](https://cruel-carlota.pagodabox.com/8827b3c64924747efec9621875b6e26d "githalytics.com")](http://githalytics.com/darksmo/chess-smoengine)

