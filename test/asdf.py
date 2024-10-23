#!python3
import sys
import os
from subprocess import check_output, CalledProcessError
from typing import List

NORMINETTE_EXECUTABLE = "norminette"

ERROR_UNRECOGNIZED_TOKEN = "Error: Unrecognized token line"
ERROR_UNRECOGNIZED_LINE = "Error: Unrecognized line ("
ERROR_STRING_LITERAL_UNTERMINATED = "String literal unterminated detected at line"

ERROR_NESTED_BRACKETS = "Error: Nested parentheses, braces or brackets are not correctly closed"
ERROR_INVALID_PREPROCESSING_DIRECTIVE = "Invalid preprocessing directive"
ERROR_EXTRA_TOKEN_ENDIF = "Extra tokens at end of #endif directive"
ERROR_INCLUDE_FILE_ARGUMENT = "Invalid file argument for #include directive"

ERROR_NO_SUCH_FILE = "no such file or directory"
ERROR_NO_VALID_C_FILE = "is not valid C or C header file"

ERROR_WITH_LINE_INFO = [ERROR_UNRECOGNIZED_LINE, ERROR_UNRECOGNIZED_TOKEN, ERROR_STRING_LITERAL_UNTERMINATED]
ERROR_WITH_LINE_INFO_TO_FIND = [ERROR_NESTED_BRACKETS, ERROR_EXTRA_TOKEN_ENDIF, ERROR_INVALID_PREPROCESSING_DIRECTIVE, ERROR_INCLUDE_FILE_ARGUMENT]
ERROR_WITHOUT_LINE_INFO = [ERROR_NO_VALID_C_FILE, ERROR_NO_SUCH_FILE]

VALID_PREPROCESSOR_DIRECTIVES = ["ifndef", "endif", "define", "include"]


def recursive_c_h_file_search(folder):
	ls = os.listdir(folder)
	files = []
	for file in ls:
		if os.path.isdir(file) and not os.path.islink(file):
			files = [*files, *recursive_c_h_file_search(os.path.join(folder, file))]
		elif(file[-2:] == ".c" or file[-2] == ".h"):
			file = os.path.join(folder, file)
			if file[:2] == "./":
				file = file[2:]
			files.append(file)
	return files

def find_from_list(line: str, needles: List[str]):
	for needle in needles:
		if (res := line.find(needle)) != -1:
			return needle
	return ""

def get_errors_from_norminette(files):
	errors = {}
	try:
		output = check_output([NORMINETTE_EXECUTABLE, *files], )
	except CalledProcessError as err:
		output = err.output
	output = output.decode("utf-8").split("\n")
	current_file = None
	for line in output:
		if line[-5:] == ": OK!" or line == "":
			continue
		elif line[-8:] == ": Error!":
			current_file = line[:-8]
		elif (error := find_from_list(line, ERROR_WITHOUT_LINE_INFO)) != "":
			key = line[:line.find(error)].replace("Error: ", "").replace("'", "").strip()
			if key not in errors:
				errors[key] = list()
				files.append(key)
			errors[key].append({"error_code": "", "error_msg": error, \
								"line": None, "column": None})
		elif (error := find_from_list(line, ERROR_WITH_LINE_INFO)) != "":
			line = line.replace("\x1b[0m", "").strip()
			line = line[line.find(error):].split(",", 1)
			line_number = int(line[0])
			column_number = None
			if len(line) == 2:
				line = line[1]
				if (bracket := line.find(")")):
					line = line[:bracket]
				column_number = int(line)
			errors[current_file].append({"error_code": "", "error_msg": error, \
								"line": line_number, "column": column_number})	
		else:
			if line.find(ERROR_UNRECOGNIZED_TOKEN) != -1:
				if not current_file in errors:
					errors[current_file] = []
				line = line.replace("\x1b[0m", "")
				line = line[line.find("line") + 5:].strip()
				line_number, col_number = line.split(",")
				col_number = col_number.strip()[len("col "):].strip()
				errors[current_file].append({"error_code": "UNRECOGNIZED_TOKEN", "error_msg": "Token was not recognized", "line": int(line_number), "column": int(col_number)})
				continue
			elif line.find(ERROR_UNRECOGNIZED_LINE) != -1:
				files.remove(current_file)
				print(current_file+ ": " + line)
				continue
			print(line)
			front, detail = line.split("):")
			code, position = front.strip().split("(")
			code = code[7:]
			position = position.split(",")
			line_number = int(position[0].split(":")[1].strip())
			column_number = int(position[1].split(":")[1].strip())
			if not current_file in errors:
				errors[current_file] = []
			errors[current_file].append({"error_code": code.strip(), "error_msg": detail.strip(), "line": line_number, "column": column_number})
	return errors

def print_file_errors(file, errors, longest_prefix, longest_error, longest_msg):
	longest_prefix = longest_prefix + 2 + 3 + 2 + 4
	longest_error = longest_error + 8
	if file in errors:
		print("=" * (longest_prefix - 4) + " " + file  + " " + "=" * (longest_error + longest_msg - len(file) + 4 - 2))
		for error in errors[file]:
			line = error["line"]
			col = error["column"]
			code = error["error_code"]
			msg = error["error_msg"]

			#code/scene.c:152:46:
			prefix = file
			if line is not None:
				prefix += ":" + str(line)
			if col is not None:
				prefix += ":" + str(col)
			tabs = " " * (longest_prefix - len(prefix))
			#tabs = "\t" * round((longest - len(prefix)) / 4 + 0.5)
			print (prefix + tabs + code + " " * (longest_error - len(code)) + msg)
	
def get_line_invalid_pp_directive(path):
	with open(path, "r", encoding="utf-8") as f:
		file = f.read().split("\n")
	for index, line in enumerate(file):
		if line[:1] == "#":
			if line[1:].strip() not in VALID_PREPROCESSOR_DIRECTIVES:
				return index, None
	return None, None

def get_line_extra_token_endif(path):
	with open(path, "r", encoding="utf-8") as f:
		file = f.read().split("\n")
	for index, line in enumerate(file):
		if line[:1] == "#":
			if line[1:].strip()[:len("endif")] == "endif" and len(line[1:].strip()) > len("endif"):
				return index, None
	return None, None

def get_line_error_include_argument(path):
	with open(path, "r", encoding="utf-8") as f:
		file = f.read().split("\n")
	for index, line in enumerate(file):
		if line[:1] == "#":
			if (line:=line[1:].strip()[:len("include")]) == "include":
				if (line[:1] != '"' and line[:1] != '<') or (line[-1:] != '"' and line[-1:] != '>'):
					return index, None
	return None, None

def get_line_error_nested_brackets(path):
	with open(path, "r", encoding="utf-8") as f:
		file = f.read().split("\n")
	stop = False
	for index, line in enumerate(file):
		single_quotes = False
		double_quotes = False
		round_brackets = []
		square_brackets = []
		for cindex, char in enumerate(line):
			if char == "'":
				if single_quotes:
					single_quotes = False
				else:
					single_quotes = True
			if char == '"':
				if double_quotes:
					double_quotes = False
				else:
					double_quotes = True
			if char == "[" and not single_quotes and not double_quotes:
				square_brackets.append(cindex)
			if char == "(" and not single_quotes and not double_quotes:
				round_brackets.append(cindex)
			if char == "]" and not single_quotes and not double_quotes:
				if square_brackets == []:
					stop = True
					break
				elif round_brackets != [] and round_brackets[-1] > square_brackets[-1]:
					stop = True
					break
				else:
					del square_brackets[-1]
			if char == ")" and not single_quotes and not double_quotes:
				if round_brackets == []:
					stop = True
					break
				elif square_brackets != [] and square_brackets[-1] > round_brackets[-1]:
					stop = True
					break
				else:
					del round_brackets[-1]
		if stop:
			return index, cindex
		elif round_brackets != []:
			return index, round_brackets[-1]
		elif square_brackets != []:
			return index, square_brackets[-1]
	return None, None	

if __name__ == "__main__":
	files = sys.argv[1:]

	if files == []:
		files = recursive_c_h_file_search(".")

	errors_from_norminette = get_errors_from_norminette(files)
	if errors_from_norminette == {}:
		exit()
	longest_path = max(len(key) for key in errors_from_norminette)
	longest_error_code = max(len(error["error_code"]) for error in [j for i in errors_from_norminette.values() for j in i])
	longest_error_msg = max(len(error["error_msg"]) for error in [j for i in errors_from_norminette.values() for j in i])
	for file in files:
		print_file_errors(file, errors_from_norminette, longest_path, longest_error_code, longest_error_msg)
	width = longest_path + longest_error_code + longest_error_msg + 2 + 3 + 2 + 4 + 8
	print(" " * width)
	error_count = sum(len(val) for key, val in errors_from_norminette.items())
	summary = f"found {error_count} {'error' if error_count == 1 else 'errors'}"
	if len(files) > 1:
		summary += f" in {len(errors_from_norminette.keys())} files"
	#print(" " * (width - len(summary)) + summary)
	print (summary)
