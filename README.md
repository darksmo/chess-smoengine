chess-smoengine
===============

Simple Chess Engine

Still in development during my free time, a chess engine with a simple
interface that can be easily integrated in your own C chess program.

Features currently implemented:
- tests for bitboards, placement of pieces, legal moves
- create bitboards out of your own representation of a chessboard
- initial structure for a test of legal moves, which checks if moves from real
  games are considered legal
- not yet optimised bitboard infrastructure to represent a chessboard:
 * (incomplete) kings movements/attacks
 * knight movements/attacks
 * rook movements/attacks
 * bishop movements/attacks
 * queen movements/attacks

Planning to:
- moves for pawn pieces
- complete king moves
- improve chessboard querying
- finish up the test for legal moves
- special moves:
 * en passant capture
 * promotion
 * castling
- search algorithm
- evaluation function
