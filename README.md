# Water Sort Puzzle
An implementation of the Breadth First Search algorithm for playing the "Water Sort" puzzle game.

### Rules of the Game
* Each state of the puzzle consists of $N$ liquid bottles with 4 mL capacity each (total space: $4N$ mL)
* A total of $N - 2$ different colors are present with 4 mL of each one being dispersed in different bottles (total liquids: $4N-8$ mL)
* Initially, the colored liquids spawn randomly, filling up the $N - 2$ first bottles and leaving the last 2 empty.
* We can choose to pour from bottle $i$ to bottle $j$ if and only if a. the two bottles have the same color on their top AND b. there is enough space in the $j$-th bottle to pour all of the continouous mL of the same color situated on the top of the $i$-th bottle (transition operator).
* Puzzle is solved when a. two bottles remain empty AND b. each bottle has all 4 mL of the same color's liquid.

<p align="center">
  <img src="./img/exhibition.gif" alt="Test Run Exhibition" width="960" height="130">
  <br>
  <i>A preview of the project's execution for 10 bottles.</i>
</p>
