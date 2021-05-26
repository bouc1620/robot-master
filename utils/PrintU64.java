/**
 * simply prints a bitboard in the console from an hex integer
 */

import java.math.BigInteger;
import java.util.Scanner;

public class PrintU64 {
	public static void main(String[] args) {
		if (args.length < 1) {
			Scanner scanner = new Scanner(System.in);

			while (true) {
				String input = scanner.nextLine();
				if (input.equals("exit"))
					break;
				print(input);
			}
		} else {
			print(args[0]);
		}
	}

	private static void print(String bitboard) {
		try {
			if (bitboard.startsWith("0x"))
				bitboard = bitboard.substring(2);
			long bb = Long.parseUnsignedLong(bitboard, 16);

			bitboard =
					String.format("%032d", new BigInteger(Long.toBinaryString(bb)));
			bitboard = String.format("%64s", bitboard).replace(' ', '0');

			System.out.println("");
			for (int i = 0; i < 8; ++i) {
				String line = bitboard.substring(i * 8, (i + 1) * 8);
				line = new StringBuilder(line).reverse().toString();
				System.out.println(8 - i + " | " + String.join(" ", line.split("")));
			}
			System.out.println("    ---------------");
			System.out.println("    a b c d e f g h\n");
		} catch (NumberFormatException e) {
			System.out.println("Invalid format");
		}
	}
}
