#pragma once
#include <string>

extern std::string const html_failed_load_archive_file = R"html(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Archive Load Failure</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background: #f8f8f8;
            margin: 0;
            padding: 0;
        }
        .container {
            max-width: 600px;
            margin: 60px auto;
            background: #fff;
            border-radius: 8px;
            box-shadow: 0 2px 8px rgba(0,0,0,0.08);
            padding: 32px 24px;
        }
        h1 {
            color: #c0392b;
            font-size: 1.5em;
            margin-bottom: 16px;
        }
        p {
            color: #333;
            font-size: 1.1em;
            margin-bottom: 12px;
        }
        .note {
            color: #555;
            font-size: 0.98em;
            background: #f9e6e6;
            border-left: 4px solid #c0392b;
            padding: 10px 16px;
            border-radius: 4px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Archive Load Failure</h1>
        <p>
            An attempt was made to load the archive of papers, but it failed to load.
        </p>
        <div class="note">
            Check that the archive file named '<b><i>all_cxx_papers_individual_zst.tar</b></i>' is present in the same directory as the <b>Paper Kernel C++</b> program.
        </div>
    </div>
</body>
</html>
)html";

extern std::string const html_failed_load_archive_resource = R"html(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Archive Load Failure</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background: #f8f8f8;
            margin: 0;
            padding: 0;
        }
        .container {
            max-width: 600px;
            margin: 60px auto;
            background: #fff;
            border-radius: 8px;
            box-shadow: 0 2px 8px rgba(0,0,0,0.08);
            padding: 32px 24px;
        }
        h1 {
            color: #c0392b;
            font-size: 1.5em;
            margin-bottom: 16px;
        }
        p {
            color: #333;
            font-size: 1.1em;
            margin-bottom: 12px;
        }
        .note {
            color: #555;
            font-size: 0.98em;
            background: #f9e6e6;
            border-left: 4px solid #c0392b;
            padding: 10px 16px;
            border-radius: 4px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Archive Load Failure</h1>
        <p>
            An attempt was made to load the archive of papers, but it failed to load.
        </p>
        <div class="note">
            The archive file is embedded as a resource in the executable file for <b>Paper Kernel C++</b>, but is failing to load. The executable file is likely corrupt. Download it again from the GitHub website.
        </div>
    </div>
</body>
</html>
)html";

extern std::string const html_paper_not_found_in_archive = R"html(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <title>Paper Retrieval Failure</title>
    <style>
        body {
            font-family: Arial, sans-serif;
            background: #f8f8f8;
            margin: 0;
            padding: 0;
        }
        .container {
            max-width: 600px;
            margin: 60px auto;
            background: #fff;
            border-radius: 8px;
            box-shadow: 0 2px 8px rgba(0,0,0,0.08);
            padding: 32px 24px;
        }
        h1 {
            color: #c0392b;
            font-size: 1.5em;
            margin-bottom: 16px;
        }
        p {
            color: #333;
            font-size: 1.1em;
            margin-bottom: 12px;
        }
        .note {
            color: #555;
            font-size: 0.98em;
            background: #f9e6e6;
            border-left: 4px solid #c0392b;
            padding: 10px 16px;
            border-radius: 4px;
        }
    </style>
</head>
<body>
    <div class="container">
        <h1>Paper Retrieval Failure</h1>
        <p>
            An attempt was made to retrieve the paper from the archive, but the paper was not found.
        </p>
        <div class="note">
            Please email Thomas on ' <b>t@virjacode.com</b> ' as there should not be any papers missing from the archive.
        </div>
    </div>
</body>
</html>
)html";