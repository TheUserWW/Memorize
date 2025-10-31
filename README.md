# Memorize - Multi-language Vocabulary Memorization Software

# Download Link
https://www.softpedia.com/get/Others/Home-Education/Memorize-TheUserWW.shtml

## Project Overview

Memorize is a powerful cross-platform vocabulary memorization software developed using the Qt framework, supporting multiple language learning modes. Designed specifically for language learners, this software provides an intuitive interface and rich features to help users efficiently memorize and master vocabulary.

## Key Features

### 🌍 Multi-language Support
- **Built-in Vocabulary Libraries**: Includes vocabulary libraries for English, Chinese, Russian, and other languages
- **Multi-language Interface**: Supports Chinese, English, Spanish, French, Russian, Arabic, Tibetan, Uyghur, Syriac, and other interface languages
- **Internationalization Design**: Complete Qt translation system for easy extension of new languages

### 📚 Rich Learning Modes
- **Vocabulary List Management**: Complete CRUD functionality for vocabulary
- **Flashcard Learning**: Interactive flashcard learning mode with card flip animations
- **Testing Functionality**: Intelligent testing system to evaluate learning effectiveness
- **Statistics Functionality**: Detailed learning progress tracking and performance analysis

### 🔊 Speech Features
- **Text-to-Speech**: Integrated QTextToSpeech with word pronunciation
- **Play Buttons**: Each word includes a pronunciation button
- **Phonetic Display**: Standard phonetic notation to assist correct pronunciation

### 🎯 Intelligent Learning System
- **Random Generation**: Smart random content generation for learning
- **Progress Tracking**: Real-time learning progress and mastery level monitoring
- **Customizable Settings**: Personalized learning ranges and difficulty levels

## Technical Architecture

### Development Framework
- **Qt 6.10+**: Cross-platform C++ application framework
- **C++17**: Modern C++ standard ensuring code quality and performance


### Core Modules
- **MainWindow**: Main interface and core logic control
- **FlashcardWidget**: Flashcard learning module
- **StatisticsWidget**: Statistics and analysis module
- **AddWordDialog**: Vocabulary management module
- **AboutDialog**: About information display

### Data Management
- **CSV Format**: Standard CSV files for vocabulary data storage
- **QSettings**: Cross-platform configuration management
- **QStandardItemModel**: Efficient data model management

## Installation & Usage


### Compilation Instructions
```bash
# Clone the project
git clone https://github.com/TheUserWW/Memorize.git
cd Memorize

# Build using qmake
qmake Memorize.pro
make

# Or open .pro file directly in Qt Creator
```

### Running Instructions
1. Ensure Qt runtime libraries are installed
2. Run the compiled executable directly
3. Configuration files are automatically created on first run

## Project Structure

```
Memorize/
├── src/                    # Source code directory
│   ├── mainwindow.*       # Main window class
│   ├── flashcard.*        # Flashcard learning module
│   ├── statistics.*       # Statistics module
│   ├── addworddialog.*    # Add word dialog
│   └── aboutdialog.*      # About dialog
├── resources/             # Resource files
│   ├── vocab/            # Vocabulary library files
│   ├── icons/            # Icon resources
│   └── translations/     # Translation files
├── build/                # Build output directory
└── docs/                 # Documentation directory
```

## Vocabulary Library Format

The software uses standard CSV format for vocabulary data storage:
```csv
"word","phonetic","part of speech","definition"
"example","/ɪɡˈzæmpl/","n.","an illustrative case"
```

## Contributing Guidelines

We welcome all forms of contributions! Please follow these steps:

1. Fork the project
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

⭐ If this project is helpful to you, please give us a star! Your support is the motivation for our continued development.
        
