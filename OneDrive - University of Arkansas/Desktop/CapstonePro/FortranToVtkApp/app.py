import os
import paramiko
from flask import Flask, request, render_template, flash, redirect, url_for

app = Flask(__name__)
app.secret_key = 'your_secret_key'  # Required for session management and flashing messages
UPLOAD_FOLDER = 'uploaded_files'  # Directory for uploaded files
app.config['UPLOAD_FOLDER'] = UPLOAD_FOLDER

# Ensure the upload folder exists
os.makedirs(UPLOAD_FOLDER, exist_ok=True)  # This will create the directory if it doesn't exist

@app.route('/')
def index():
    return render_template('index.html')

# Route for handling file uploads
@app.route('/upload', methods=['POST'])
def upload_files():
    if 'files' not in request.files:
        flash('No file part', 'error')
        return redirect(url_for('index'))
    
    files = request.files.getlist('files')
    if not files:
        flash('No files selected', 'error')
        return redirect(url_for('index'))

    for file in files:
        if file.filename == '':
            flash('No selected file', 'error')
            return redirect(url_for('index'))
        
        filename = file.filename
        filepath = os.path.join(app.config['UPLOAD_FOLDER'], filename)
        file.save(filepath)

    # Now run the shell script on HPC via SSH
    ssh_and_execute_script(files)
    flash('Files uploaded and script executed successfully', 'success')
    return redirect(url_for('index'))

def ssh_and_execute_script(files):
    ssh = paramiko.SSHClient()
    ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())

    try:
        # Get the user's credentials (consider using environment variables or a safer method for production)
        hostname = 'pinnacle-l2.uark.edu'
        username = input("Enter your HPC username: ")
        password = input("Enter your HPC password: ")

        # Connect to HPC via SSH
        ssh.connect(hostname, username=username, password=password)

        # Upload the Fortran files
        sftp = ssh.open_sftp()
        for file in files:
            sftp.put(os.path.join(app.config['UPLOAD_FOLDER'], file.filename), f'/remote/directory/{file.filename}')
        sftp.close()

        # Run the shell script remotely
        stdin, stdout, stderr = ssh.exec_command('bash /remote/directory/run_fortran.sh')
        output = stdout.read().decode()
        print(output)

    finally:
        ssh.close()

if __name__ == "__main__":
    app.run(debug=True)
