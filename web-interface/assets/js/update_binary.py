code_to_replace1 = ",put_char:function(tty,val){if(val===null||val===10){out"
correct_code1 = ",put_char:function(tty,val){if(val===null||val===10){console.log"

code_to_replace2 = "var shouldRunNow=true"
correct_code2 = "var shouldRunNow=false"

sobstitutions = {code_to_replace1: correct_code1, code_to_replace2: correct_code2}

with open("./code_to_append.js", 'r', encoding='utf-8') as file:
    code_to_append = file.read()

file_path = "./binary.js"

try:
    with open(file_path, 'r', encoding='utf-8') as file:
        content = file.read()

    for old_code, new_code in sobstitutions.items():
        content = content.replace(old_code, new_code)

    content += "\n" + code_to_append

    with open(file_path, 'w', encoding='utf-8') as file:
        file.write(content)

    print(f"modifiche al file {file_path} effettuate con successo.")

except FileNotFoundError:
    print(f"{file_path} non trovato.")
except Exception as e:
    print(f"errore: {e}")