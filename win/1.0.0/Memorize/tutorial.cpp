#include "tutorial.h"
#include "ui_tutorial.h"
#include <QMessageBox>

Tutorial::Tutorial(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Tutorial),
    currentPage(0)
{
    ui->setupUi(this);
    setWindowTitle(tr("Memorize User Guide"));  // 窗口标题国际化
    resize(800, 600);  // 增大窗口尺寸以提升可读性

    // 初始化教程页面标题（使用tr包裹）
    pageTitles <<
        tr("Welcome to Memorize") <<
        tr("Adding New Words") <<
        tr("Using Flashcards for Learning") <<
        tr("Testing Your Knowledge") <<
        tr("Viewing Learning Statistics");

    // 初始化每页详细内容（使用tr包裹）
    pageContents <<
        // 第1页：欢迎
        tr("Welcome to Memorize - your personal vocabulary learning assistant!\n\n"
           "This guide will help you master all features of the software, designed to enhance your vocabulary retention through scientific learning methods. Whether you're learning a new language or expanding your native vocabulary, Memorize provides the tools you need to succeed.\n\n"
           "Key features include:\n"
           "- Custom word lists with detailed definitions\n"
           "- Interactive flashcards for active recall practice\n"
           "- Multiple test modes to assess your progress\n"
           "- Comprehensive statistics to track your growth\n\n"
           "Click 'Next' to explore each feature in detail.")

                 // 第2页：添加单词
                 << tr("Adding New Words\n\n"
                       "Build your personalized vocabulary list by adding new words with detailed information:\n\n"
                       "1. Access the Add Word function:\n"
                       "   - Click the 'Edit' menu in the top navigation bar.\n"
                       "   - Select 'Add Word' from the dropdown menu. (Shortcut: Ctrl+N)\n\n"
                       "2. Fill in the word details:\n"
                       "   - Word: Enter the correct spelling (e.g., 'aberration').\n"
                       "   - Phonetic: Add pronunciation (e.g., /ˌæbəˈreɪʃn/ for English).\n"
                       "   - Part of Speech: Specify grammar category (e.g., noun, verb).\n"
                       "   - Translation/Meaning: Provide definitions or translations (e.g., 'a deviation from the normal').\n\n"
                       "3. Save the word:\n"
                       "   - Click 'OK' to add it to your list. Empty 'Word' or 'Meaning' fields will trigger an error.\n"
                       "   - Click 'Cancel' to discard the entry.\n\n"
                       "New words are automatically saved to your current vocabulary file.")

                 // 第3页：闪卡学习
                 << tr("Using Flashcards for Learning\n\n"
                       "Flashcards leverage active recall to strengthen memory retention. Follow these steps:\n\n"
                       "1. Access Flashcard Mode:\n"
                       "   - Click the 'Study' menu and select 'Flashcard Mode'.\n"
                       "   - Set the range of words to study using the spin boxes (1-based index).\n"
                       "   - Click 'Generate' to create your flashcard set.\n\n"
                       "2. Interacting with flashcards:\n"
                       "   - Flip: Click the card or press Spacebar to toggle between front (word) and back (details).\n"
                       "   - Pronunciation: Click the speaker icon on the front to hear the word.\n"
                       "   - Navigation: Use 'Previous' (←) and 'Next' (→) buttons to move between cards.\n"
                       "   - Exit: Click 'Back to Home' to return to the main word list.\n\n"
                       "3. Learning tips:\n"
                       "   - Try recalling the meaning before flipping the card.\n"
                       "   - Repeat difficult words by navigating back.\n"
                       "   - Use the pronunciation feature to practice listening skills.")

                 // 第4页：测试功能
                 << tr("Testing Your Knowledge\n\n"
                       "Assess your vocabulary retention with customizable tests:\n\n"
                       "1. Start a test:\n"
                       "   - Click the 'Test' menu and select 'Start Test'.\n"
                       "   - Ensure you have at least 4 words in your list (required for valid testing).\n\n"
                       "2. Configure test settings:\n"
                       "   - Test Mode: Choose from 4 types:\n"
                       "     • Word → Meaning: Select the correct definition for a given word.\n"
                       "     • Meaning → Word: Select the correct word for a given definition.\n"
                       "     • Spelling Test: Type the word matching a given meaning.\n"
                       "     • Typing Test: Type the word you hear (uses text-to-speech).\n"
                       "   - Question Count: Set the number of questions (default: 10).\n"
                       "   - Time Limit: Set a timer in seconds (0 = no limit).\n\n"
                       "3. Taking the test:\n"
                       "   - For multiple-choice: Click your answer.\n"
                       "   - For typing: Enter your answer and click 'Submit'.\n"
                       "   - Progress: Track your position with the 'Question X/Y' label.\n\n"
                       "4. View results:\n"
                       "   - After completion, see your score, percentage, and time used.\n"
                       "   - Review a detailed table of all questions, including correct answers.")

                 // 第5页：统计功能
                 << tr("Viewing Learning Statistics\n\n"
                       "Track your progress and identify areas for improvement:\n\n"
                       "1. Access Statistics:\n"
                       "   - Click the 'Statistics' menu and select 'View Statistics'.\n\n"
                       "2. Key metrics explained:\n"
                       "   - Total Tests: Number of tests completed.\n"
                       "   - Total Words Tested: Cumulative count of words across all tests.\n"
                       "   - Cumulative Accuracy: Average score across all tests (%).\n"
                       "   - Highest Accuracy: Your best single-test performance (%).\n\n"
                       "3. Progress Chart:\n"
                       "   - A line graph shows accuracy trends over time (X-axis: dates; Y-axis: %).\n"
                       "   - Hover over points to see exact values.\n\n"
                       "4. Using statistics effectively:\n"
                       "   - Identify trends (e.g., improving accuracy indicates effective learning).\n"
                       "   - Focus on words with frequent errors (review via flashcards).\n"
                       "   - Set goals based on your cumulative accuracy (e.g., target 80%).\n\n"
                       "Data is automatically saved to 'Memorize.ini' in the application directory.");



    // 显示第一页内容
    updatePageContent();
}

Tutorial::~Tutorial()
{
    delete ui;
}

void Tutorial::on_nextButton_clicked()
{
    if (currentPage < pageTitles.size() - 1) {
        currentPage++;
        updatePageContent();
    } else {
        // 教程完成提示（使用tr包裹）
        QMessageBox::information(this, tr("Guide Complete"), tr("You've finished the tutorial! Start using Memorize to enhance your vocabulary."));
        close();
    }
}

void Tutorial::on_prevButton_clicked()
{
    if (currentPage > 0) {
        currentPage--;
        updatePageContent();
    }
}

void Tutorial::on_closeButton_clicked()
{
    close();
}

void Tutorial::updatePageContent()
{
    // 更新标题、内容和页码指示器（使用tr包裹动态文本）
    ui->titleLabel->setText(pageTitles[currentPage]);
    ui->contentTextEdit->setText(pageContents[currentPage]);
    ui->pageLabel->setText(tr("Page %1 of %2").arg(currentPage + 1).arg(pageTitles.size()));

    // 更新按钮状态和文本（使用tr包裹）
    ui->prevButton->setEnabled(currentPage > 0);
    ui->nextButton->setText((currentPage == pageTitles.size() - 1) ? tr("Finish") : tr("Next"));
}
