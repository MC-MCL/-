#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <ctime>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <filesystem>
#include <map>
#include <functional>
#include <memory>
#include <nlohmann/json.hpp>  // 需要安装：vcpkg install nlohmann-json

using json = nlohmann::json;
namespace fs = std::filesystem;

// ============================================================
// 1. Stats - 核心属性
// ============================================================
class Stats {
private:
    std::string name;
    int age;
    std::string major;
    int semester = 1;
    double gpa = 0.0;
    int energy = 80;
    int happiness = 70;
    int money = 500;
    std::map<std::string, int> buffs;  // buff名称 → 剩余天数

public:
    // ----- Getter -----
    std::string getName() const { return name; }
    int getAge() const { return age; }
    std::string getMajor() const { return major; }
    int getSemester() const { return semester; }
    double getGPA() const { return gpa; }
    int getEnergy() const { return energy; }
    int getHappiness() const { return happiness; }
    int getMoney() const { return money; }

    // ----- Setter -----
    void setName(const std::string& n) { name = n; }
    void setGPA(double g) { gpa = std::clamp(g, 0.0, 4.0); }

    // ----- 状态修改 -----
    void modifyEnergy(int delta) { 
        energy = std::clamp(energy + delta, 0, 100);
        if (energy == 0) std::cout << "⚠️ 精力耗尽！\n";
    }
    
    void modifyHappiness(int delta) { 
        happiness = std::clamp(happiness + delta, 0, 100);
        if (happiness == 0) std::cout << "⚠️ 太不开心了！\n";
    }
    
    void modifyMoney(int delta) { money = std::max(0, money + delta); }
    void nextSemester() { semester++; }

    // ----- Buff系统 -----
    void addBuff(const std::string& name, int days) {
        buffs[name] = days;
    }
    
    bool hasBuff(const std::string& name) const {
        return buffs.find(name) != buffs.end() && buffs.at(name) > 0;
    }
    
    void updateBuffs() {
        for (auto it = buffs.begin(); it != buffs.end();) {
            it->second--;
            if (it->second <= 0) {
                it = buffs.erase(it);
            } else {
                ++it;
            }
        }
    }
    
    int getBuffBonus(const std::string& type) const {
        if (hasBuff("学习效率+20%") && type == "study") return 20;
        if (hasBuff("精力恢复+50%") && type == "rest") return 50;
        return 0;
    }

    // ----- 序列化 -----
    json toJson() const {
        return {
            {"name", name},
            {"age", age},
            {"major", major},
            {"semester", semester},
            {"gpa", gpa},
            {"energy", energy},
            {"happiness", happiness},
            {"money", money},
            {"buffs", buffs}
        };
    }
    
    void fromJson(const json& j) {
        name = j.value("name", "");
        age = j.value("age", 18);
        major = j.value("major", "");
        semester = j.value("semester", 1);
        gpa = j.value("gpa", 0.0);
        energy = j.value("energy", 80);
        happiness = j.value("happiness", 70);
        money = j.value("money", 500);
        if (j.contains("buffs")) {
            buffs = j["buffs"].get<std::map<std::string, int>>();
        }
    }
};

// ============================================================
// 2. Course - 课程
// ============================================================
class Course {
private:
    std::string name;
    int credit;
    int score = 0;
    std::string grade = "F";
    bool isPassed = false;

public:
    Course() = default;
    Course(std::string n, int c) : name(n), credit(c) {}

    void setScore(int s) {
        score = std::clamp(s, 0, 100);
        if (score >= 90) grade = "A";
        else if (score >= 80) grade = "B";
        else if (score >= 70) grade = "C";
        else if (score >= 60) grade = "D";
        else grade = "F";
        isPassed = (grade != "F");
    }

    // ----- Getter -----
    std::string getName() const { return name; }
    int getCredit() const { return credit; }
    int getScore() const { return score; }
    std::string getGrade() const { return grade; }
    bool getIsPassed() const { return isPassed; }

    // ----- 序列化 -----
    json toJson() const {
        return {{"name", name}, {"credit", credit}, {"score", score}, {"grade", grade}, {"passed", isPassed}};
    }
    
    void fromJson(const json& j) {
        name = j.value("name", "");
        credit = j.value("credit", 3);
        score = j.value("score", 0);
        grade = j.value("grade", "F");
        isPassed = j.value("passed", false);
    }
};

// ============================================================
// 3. CourseManager - 课程管理
// ============================================================
class CourseManager {
private:
    std::vector<Course> courses;
    std::vector<Course> availableCourses;
    std::mt19937 rng;

public:
    CourseManager() {
        rng.seed(std::time(nullptr));
        
        // 初始化课程库
        availableCourses = {
            {"高等数学", 4}, {"C++编程", 3}, {"数据结构", 3},
            {"操作系统", 3}, {"计算机网络", 3}, {"数据库原理", 3},
            {"软件工程", 2}, {"人工智能", 4}, {"机器学习", 3},
            {"英语", 2}, {"离散数学", 3}, {"编译原理", 3}
        };
    }

    std::vector<Course> getAvailableCourses() const { return availableCourses; }
    std::vector<Course> getCourses() const { return courses; }
    int getCourseCount() const { return courses.size(); }

    bool takeCourse(const std::string& name) {
        for (auto& c : availableCourses) {
            if (c.getName() == name) {
                // 检查是否已选过
                for (const auto& enrolled : courses) {
                    if (enrolled.getName() == name) return false;
                }
                courses.push_back(c);
                return true;
            }
        }
        return false;
    }

    void studyForExam(const std::string& name, int hours, Stats& stats) {
        for (auto& course : courses) {
            if (course.getName() == name) {
                std::uniform_int_distribution<int> baseDist(50, 70);
                int baseScore = baseDist(rng);
                
                int bonus = std::min(hours * 3, 30);
                
                // 检查buff
                if (stats.hasBuff("学习效率+20%")) {
                    bonus = static_cast<int>(bonus * 1.2);
                }
                
                if (stats.getEnergy() < 30) {
                    bonus = static_cast<int>(bonus * 0.5);
                    std::cout << "⚠️ 精力不足，学习效果减半！\n";
                }
                
                int finalScore = std::min(baseScore + bonus, 100);
                course.setScore(finalScore);
                
                stats.modifyEnergy(-hours * 2);
                stats.modifyHappiness(3);
                
                std::cout << "📊 " << name << " 成绩: " << finalScore 
                          << " (" << course.getGrade() << ")\n";
                return;
            }
        }
        std::cout << "❌ 未找到课程: " << name << "\n";
    }

    void examWeek(Stats& stats) {
        std::cout << "\n===== 📝 考试周 =====\n";
        for (auto& course : courses) {
            if (course.getScore() == 0) {
                std::uniform_int_distribution<int> dist(1, 3);
                int hours = dist(rng);
                studyForExam(course.getName(), hours, stats);
            }
        }
    }

    double calculateGPA() const {
        if (courses.empty()) return 0.0;
        double totalPoints = 0.0;
        int totalCredits = 0;
        for (const auto& course : courses) {
            std::string grade = course.getGrade();
            double points = 0.0;
            if (grade == "A") points = 4.0;
            else if (grade == "B") points = 3.0;
            else if (grade == "C") points = 2.0;
            else if (grade == "D") points = 1.0;
            totalPoints += points * course.getCredit();
            totalCredits += course.getCredit();
        }
        return totalCredits > 0 ? totalPoints / totalCredits : 0.0;
    }

    // ----- 序列化 -----
    json toJson() const {
        json j;
        for (const auto& c : courses) j.push_back(c.toJson());
        return j;
    }
    
    void fromJson(const json& j) {
        courses.clear();
        for (const auto& item : j) {
            Course c;
            c.fromJson(item);
            courses.push_back(c);
        }
    }
};

// ============================================================
// 4. SkillManager - 技能系统
// ============================================================
class SkillManager {
private:
    std::vector<std::string> skills;
    std::map<std::string, std::string> skillDescriptions;

public:
    SkillManager() {
        skillDescriptions = {
            {"编程能力", "写代码更快更好"},
            {"数学思维", "逻辑推理能力提升"},
            {"英语应用", "阅读外文资料无障碍"},
            {"数据分析", "处理数据更加高效"},
            {"项目管理", "组织协调能力增强"},
            {"演讲能力", "口才与表达出众"},
            {"团队协作", "善于与他人合作"}
        };
    }

    void unlockSkill(const std::string& name) {
        if (std::find(skills.begin(), skills.end(), name) == skills.end()) {
            skills.push_back(name);
            std::cout << "🎯 获得新技能: " << name << "！\n";
        }
    }

    bool hasSkill(const std::string& name) const {
        return std::find(skills.begin(), skills.end(), name) != skills.end();
    }

    std::vector<std::string> getSkills() const { return skills; }
    int getSkillCount() const { return skills.size(); }

    // ----- 序列化 -----
    json toJson() const { return skills; }
    void fromJson(const json& j) { skills = j.get<std::vector<std::string>>(); }
};

// ============================================================
// 5. RandomEvent - 随机事件
// ============================================================
struct RandomEvent {
    std::string title;
    std::string description;
    std::function<void(Stats&, CourseManager&, SkillManager&)> effect;
    int weight;
};

class EventSystem {
private:
    std::vector<RandomEvent> events;
    std::mt19937 rng;

public:
    EventSystem() {
        rng.seed(std::time(nullptr));
        
        events = {
            {
                "📚 捡到笔记",
                "你在图书馆捡到一本高数笔记，学习效率提升！",
                [](Stats& stats, CourseManager& cm, SkillManager& sm) {
                    stats.addBuff("学习效率+20%", 3);
                    stats.modifyHappiness(5);
                },
                15
            },
            {
                "🎮 室友通宵",
                "室友通宵打游戏，你的睡眠被打扰了！",
                [](Stats& stats, CourseManager& cm, SkillManager& sm) {
                    stats.modifyEnergy(-15);
                    stats.modifyHappiness(-5);
                },
                10
            },
            {
                "💼 找到兼职",
                "你找到了一份不错的兼职，赚了一些零花钱！",
                [](Stats& stats, CourseManager& cm, SkillManager& sm) {
                    stats.modifyMoney(300);
                    stats.modifyHappiness(10);
                },
                12
            },
            {
                "📝 期中考试",
                "提前公布期中考试，你有一周时间准备！",
                [](Stats& stats, CourseManager& cm, SkillManager& sm) {
                    stats.modifyEnergy(-10);
                    stats.modifyHappiness(-5);
                    cm.examWeek(stats);
                },
                8
            },
            {
                "🏆 竞赛获奖",
                "你参加了编程竞赛并获奖！",
                [](Stats& stats, CourseManager& cm, SkillManager& sm) {
                    stats.modifyHappiness(20);
                    sm.unlockSkill("编程能力");
                    stats.modifyMoney(500);
                },
                5
            },
            {
                "☕ 咖啡店偶遇",
                "你在咖啡店遇到了一位学长，给了你很多建议！",
                [](Stats& stats, CourseManager& cm, SkillManager& sm) {
                    stats.modifyHappiness(15);
                    if (!sm.hasSkill("项目管理")) {
                        sm.unlockSkill("项目管理");
                    }
                },
                10
            },
            {
                "😷 生病了",
                "你感冒了，精力大幅下降！",
                [](Stats& stats, CourseManager& cm, SkillManager& sm) {
                    stats.modifyEnergy(-20);
                    stats.modifyHappiness(-10);
                },
                6
            },
            {
                "🏃 晨跑习惯",
                "你养成了晨跑习惯，精力恢复更快！",
                [](Stats& stats, CourseManager& cm, SkillManager& sm) {
                    stats.addBuff("精力恢复+50%", 5);
                    stats.modifyHappiness(10);
                },
                8
            }
        };
    }

    bool triggerRandomEvent(Stats& stats, CourseManager& cm, SkillManager& sm) {
        int totalWeight = 0;
        for (const auto& e : events) totalWeight += e.weight;
        
        std::uniform_int_distribution<int> dist(0, totalWeight - 1);
        int roll = dist(rng);
        
        int cumulative = 0;
        for (const auto& e : events) {
            cumulative += e.weight;
            if (roll < cumulative) {
                std::cout << "\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
                std::cout << "🎲 " << e.title << "\n";
                std::cout << "📖 " << e.description << "\n";
                std::cout << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
                e.effect(stats, cm, sm);
                return true;
            }
        }
        return false;
    }
};

// ============================================================
// 6. NPC - NPC好感度系统
// ============================================================
class NPC {
private:
    std::string name;
    int favorability = 0;
    std::vector<std::string> dialogues;
    std::map<int, std::pair<std::string, std::function<void(Stats&, CourseManager&)>>> rewards;
    std::mt19937 rng;

public:
    NPC(const std::string& n) : name(n) {
        rng.seed(std::time(nullptr) + std::hash<std::string>{}(n));
        
        if (name == "学姐") {
            dialogues = {
                "你好呀！需要帮忙吗？",
                "学习要劳逸结合哦！",
                "这学期有什么目标吗？",
                "加油，我看好你！"
            };
            rewards[30] = {"📖 复习资料", [](Stats& stats, CourseManager& cm) {
                stats.addBuff("学习效率+20%", 2);
                stats.modifyHappiness(5);
            }};
            rewards[60] = {"📝 考试重点", [](Stats& stats, CourseManager& cm) {
                // 随机给一门课加10分
                auto courses = cm.getCourses();
                if (!courses.empty()) {
                    int idx = std::rand() % courses.size();
                    std::cout << "✅ 获得了 " << courses[idx].getName() << " 的考试重点！\n";
                }
            }};
        } else if (name == "室友") {
            dialogues = {
                "打游戏吗？",
                "食堂新开了家店，一起去？",
                "晚上熬夜复习？我陪你！",
                "兄弟，该交作业了！"
            };
            rewards[30] = {"🍜 一起吃饭", [](Stats& stats, CourseManager& cm) {
                stats.modifyEnergy(10);
                stats.modifyHappiness(15);
            }};
            rewards[60] = {"📚 组队学习", [](Stats& stats, CourseManager& cm) {
                stats.modifyEnergy(5);
                stats.modifyHappiness(10);
                stats.addBuff("学习效率+20%", 1);
            }};
        } else if (name == "导师") {
            dialogues = {
                "最近学习怎么样？",
                "有什么问题随时问我。",
                "你的潜力很大！",
                "毕业后有什么打算？"
            };
            rewards[30] = {"💡 学术指导", [](Stats& stats, CourseManager& cm) {
                stats.modifyHappiness(10);
            }};
            rewards[60] = {"📜 推荐信", [](Stats& stats, CourseManager& cm) {
                stats.modifyHappiness(20);
                std::cout << "🎓 获得导师推荐信！毕业竞争力大增！\n";
            }};
        }
    }

    std::string getName() const { return name; }
    int getFavorability() const { return favorability; }

    void interact(Stats& stats, CourseManager& cm) {
        // 随机对话
        std::uniform_int_distribution<int> dist(0, dialogues.size() - 1);
        std::cout << "💬 " << name << ": \"" << dialogues[dist(rng)] << "\"\n";
        
        // 增加好感度
        int increase = 5 + (std::rand() % 10);
        favorability = std::min(100, favorability + increase);
        std::cout << "❤️ " << name << " 好感度 +" << increase << " (当前: " << favorability << ")\n";
        
        // 检查奖励
        for (const auto& [threshold, reward] : rewards) {
            if (favorability >= threshold && favorability - increase < threshold) {
                std::cout << "🎁 解锁奖励: " << reward.first << "\n";
                reward.second(stats, cm);
            }
        }
    }

    // ----- 序列化 -----
    json toJson() const {
        return {{"name", name}, {"favorability", favorability}};
    }
    
    void fromJson(const json& j) {
        name = j.value("name", "");
        favorability = j.value("favorability", 0);
    }
};

class NPCManager {
private:
    std::vector<NPC> npcs;

public:
    NPCManager() {
        npcs.push_back(NPC("学姐"));
        npcs.push_back(NPC("室友"));
        npcs.push_back(NPC("导师"));
    }

    std::vector<NPC>& getNPCs() { return npcs; }

    void interactWithNPC(int index, Stats& stats, CourseManager& cm) {
        if (index >= 0 && index < npcs.size()) {
            npcs[index].interact(stats, cm);
        }
    }

    // ----- 序列化 -----
    json toJson() const {
        json j;
        for (const auto& npc : npcs) j.push_back(npc.toJson());
        return j;
    }
    
    void fromJson(const json& j) {
        for (size_t i = 0; i < j.size() && i < npcs.size(); i++) {
            npcs[i].fromJson(j[i]);
        }
    }
};

// ============================================================
// 7. Interactable - 建筑交互（多态）
// ============================================================
class Interactable {
public:
    virtual ~Interactable() = default;
    virtual std::string getName() const = 0;
    virtual sf::FloatRect getBounds() const = 0;
    virtual void interact(Stats& stats, CourseManager& cm) = 0;
    virtual sf::Color getColor() const = 0;
    virtual std::string getEmoji() const = 0;
};

class Dormitory : public Interactable {
public:
    std::string getName() const override { return "宿舍"; }
    sf::FloatRect getBounds() const override { return {100, 500, 80, 60}; }
    sf::Color getColor() const override { return sf::Color(139, 69, 19); }
    std::string getEmoji() const override { return "🏠"; }
    
    void interact(Stats& stats, CourseManager& cm) override {
        int bonus = stats.hasBuff("精力恢复+50%") ? 60 : 40;
        stats.modifyEnergy(bonus);
        stats.modifyHappiness(5);
        std::cout << "😴 睡了一觉，精力 +" << bonus << "！\n";
    }
};

class Library : public Interactable {
public:
    std::string getName() const override { return "图书馆"; }
    sf::FloatRect getBounds() const override { return {250, 500, 80, 60}; }
    sf::Color getColor() const override { return sf::Color(70, 130, 180); }
    std::string getEmoji() const override { return "📚"; }
    
    void interact(Stats& stats, CourseManager& cm) override {
        auto courses = cm.getCourses();
        if (!courses.empty()) {
            std::cout << "📖 在图书馆学习...\n";
            cm.studyForExam(courses[0].getName(), 2, stats);
        } else {
            std::cout << "⚠️ 请先选修课程！\n";
        }
    }
};

class Classroom : public Interactable {
public:
    std::string getName() const override { return "教室"; }
    sf::FloatRect getBounds() const override { return {400, 500, 80, 60}; }
    sf::Color getColor() const override { return sf::Color(255, 140, 0); }
    std::string getEmoji() const override { return "🏫"; }
    
    void interact(Stats& stats, CourseManager& cm) override {
        if (!cm.getCourses().empty()) {
            std::cout << "📝 参加考试周...\n";
            cm.examWeek(stats);
        } else {
            std::cout << "⚠️ 请先选修课程！\n";
        }
    }
};

class Canteen : public Interactable {
public:
    std::string getName() const override { return "食堂"; }
    sf::FloatRect getBounds() const override { return {550, 500, 80, 60}; }
    sf::Color getColor() const override { return sf::Color(255, 69, 0); }
    std::string getEmoji() const override { return "🍜"; }
    
    void interact(Stats& stats, CourseManager& cm) override {
        stats.modifyEnergy(20);
        stats.modifyHappiness(8);
        stats.modifyMoney(-20);
        std::cout << "🍜 吃了顿饭，精力 +20，快乐 +8，花费 20元\n";
    }
};

class Gym : public Interactable {
public:
    std::string getName() const override { return "健身房"; }
    sf::FloatRect getBounds() const override { return {700, 500, 80, 60}; }
    sf::Color getColor() const override { return sf::Color(50, 205, 50); }
    std::string getEmoji() const override { return "💪"; }
    
    void interact(Stats& stats, CourseManager& cm) override {
        stats.modifyEnergy(15);
        stats.modifyHappiness(10);
        std::cout << "💪 运动了一下，精力 +15，快乐 +10\n";
    }
};

// ============================================================
// 8. SaveManager - JSON存档
// ============================================================
class SaveManager {
private:
    std::string saveDirectory;
    std::string currentSlot = "autosave";

public:
    SaveManager() {
#ifdef _WIN32
        saveDirectory = std::string(getenv("APPDATA")) + "\\StudentSimulator\\saves\\";
#elif __APPLE__
        saveDirectory = std::string(getenv("HOME")) + "/Library/Application Support/StudentSimulator/saves/";
#else
        saveDirectory = std::string(getenv("HOME")) + "/.student_simulator/saves/";
#endif
        fs::create_directories(saveDirectory);
    }

    void setSlot(const std::string& slot) { currentSlot = slot; }

    bool save(const Stats& stats, const CourseManager& cm, 
              const SkillManager& sm, const NPCManager& npcm) {
        json data = {
            {"stats", stats.toJson()},
            {"courses", cm.toJson()},
            {"skills", sm.toJson()},
            {"npcs", npcm.toJson()},
            {"save_time", std::time(nullptr)}
        };
        
        std::string filename = saveDirectory + currentSlot + ".json";
        std::ofstream file(filename);
        if (!file.is_open()) return false;
        file << data.dump(4);  // 缩进4空格，人类可读
        file.close();
        std::cout << "💾 存档成功: " << filename << "\n";
        return true;
    }

    bool load(Stats& stats, CourseManager& cm, 
              SkillManager& sm, NPCManager& npcm) {
        std::string filename = saveDirectory + currentSlot + ".json";
        if (!fs::exists(filename)) return false;
        
        std::ifstream file(filename);
        if (!file.is_open()) return false;
        
        json data;
        file >> data;
        file.close();
        
        stats.fromJson(data["stats"]);
        cm.fromJson(data["courses"]);
        sm.fromJson(data["skills"]);
        npcm.fromJson(data["npcs"]);
        
        std::cout << "💾 读档成功: " << filename << "\n";
        return true;
    }

    std::vector<std::string> listSaves() {
        std::vector<std::string> saves;
        for (const auto& entry : fs::directory_iterator(saveDirectory)) {
            if (entry.path().extension() == ".json") {
                saves.push_back(entry.path().stem().string());
            }
        }
        return saves;
    }

    bool deleteSave(const std::string& slot) {
        std::string filename = saveDirectory + slot + ".json";
        if (!fs::exists(filename)) return false;
        return fs::remove(filename);
    }
};

// ============================================================
// 9. GameManager - 游戏管理器
// ============================================================
class GameManager {
private:
    sf::RenderWindow window;
    
    Stats stats;
    CourseManager courseManager;
    SkillManager skillManager;
    NPCManager npcManager;
    EventSystem eventSystem;
    SaveManager saveManager;
    
    std::vector<std::unique_ptr<Interactable>> buildings;
    std::vector<std::string> messages;
    
    bool isRunning = true;
    bool isInputting = false;
    std::string inputBuffer;
    std::string inputPrompt;
    
    sf::Font font;
    sf::CircleShape playerIcon;
    
    // 界面元素
    sf::RectangleShape statusPanel;
    sf::RectangleShape menuPanel;
    sf::RectangleShape messagePanel;

public:
    GameManager() {
        window.create(sf::VideoMode(1024, 768), "🎓 学生生活模拟器 v3.0", sf::Style::Close);
        window.setFramerateLimit(60);
        
        // 加载字体（跨平台）
        if (!loadFont()) {
            std::cerr << "⚠️ 字体加载失败，使用默认字体\n";
        }
        
        // 初始化建筑
        buildings.push_back(std::make_unique<Dormitory>());
        buildings.push_back(std::make_unique<Library>());
        buildings.push_back(std::make_unique<Classroom>());
        buildings.push_back(std::make_unique<Canteen>());
        buildings.push_back(std::make_unique<Gym>());
        
        // 初始化UI
        setupUI();
        
        // 初始化玩家图标
        playerIcon.setRadius(15);
        playerIcon.setFillColor(sf::Color::Cyan);
        playerIcon.setPosition(400, 300);
        playerIcon.setOutlineColor(sf::Color::White);
        playerIcon.setOutlineThickness(2);
        
        messages.push_back("🎓 欢迎来到学生生活模拟器 v3.0");
        messages.push_back("✨ 新功能: 随机事件 | NPC好感 | 技能系统");
        messages.push_back("🏠 方向键移动 | Enter交互 | 1-9菜单");
        messages.push_back("💾 S存档 | L读档 | R改名 | E触发随机事件");
    }

    bool loadFont() {
        std::vector<std::string> fontPaths = {
            "C:\\Windows\\Fonts\\msyh.ttc",
            "C:\\Windows\\Fonts\\simhei.ttf",
            "C:\\Windows\\Fonts\\arial.ttf",
            "/usr/share/fonts/truetype/noto/NotoSansCJK-Regular.ttc",
            "/usr/share/fonts/truetype/wqy/wqy-microhei.ttc",
            "/System/Library/Fonts/PingFang.ttc"
        };
        
        for (const auto& path : fontPaths) {
            if (fs::exists(path) && font.loadFromFile(path)) {
                return true;
            }
        }
        return font.loadFromFile("");
    }

    void setupUI() {
        statusPanel.setSize({300, 220});
        statusPanel.setPosition({10, 10});
        statusPanel.setFillColor({50, 50, 50, 230});
        statusPanel.setOutlineColor(sf::Color::White);
        statusPanel.setOutlineThickness(1);
        
        menuPanel.setSize({300, 380});
        menuPanel.setPosition({10, 240});
        menuPanel.setFillColor({50, 50, 50, 230});
        menuPanel.setOutlineColor(sf::Color::White);
        menuPanel.setOutlineThickness(1);
        
        messagePanel.setSize({700, 150});
        messagePanel.setPosition({320, 600});
        messagePanel.setFillColor({50, 50, 50, 230});
        messagePanel.setOutlineColor(sf::Color::White);
        messagePanel.setOutlineThickness(1);
    }

    void run() {
        // 检查是否有存档
        auto saves = saveManager.listSaves();
        if (!saves.empty()) {
            std::cout << "📂 检测到存档: ";
            for (const auto& s : saves) std::cout << s << " ";
            std::cout << "\n是否加载存档? (y/n): ";
            char choice;
            std::cin >> choice;
            if (choice == 'y' || choice == 'Y') {
                if (saveManager.load(stats, courseManager, skillManager, npcManager)) {
                    std::cout << "✅ 加载成功！\n";
                }
            }
        }
        
        // 如果还没创建角色
        if (stats.getName().empty()) {
            createCharacter();
        }
        
        while (window.isOpen() && isRunning) {
            handleEvents();
            update();
            render();
        }
    }

    void createCharacter() {
        std::string name, major;
        int age;
        std::cout << "\n🎓 创建角色\n";
        std::cout << "请输入姓名: ";
        std::cin >> name;
        std::cout << "请输入年龄: ";
        std::cin >> age;
        std::cout << "请输入专业: ";
        std::cin >> major;
        
        stats.setName(name);
        // age 和 major 需要添加setter
    }

    void handleEvents() {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                saveManager.save(stats, courseManager, skillManager, npcManager);
                window.close();
                isRunning = false;
            }
            
            if (event.type == sf::Event::KeyPressed) {
                handleKeyPress(event.key.code);
            }
            
            if (event.type == sf::Event::TextEntered && isInputting) {
                if (event.text.unicode == 13) {
                    processInput();
                } else if (event.text.unicode == 8) {
                    if (!inputBuffer.empty()) inputBuffer.pop_back();
                } else if (event.text.unicode < 128) {
                    inputBuffer += static_cast<char>(event.text.unicode);
                }
            }
        }
    }

    void handleKeyPress(sf::Keyboard::Key key) {
        if (isInputting) return;
        
        float speed = 5.0f;
        auto pos = playerIcon.getPosition();
        if (key == sf::Keyboard::Up) pos.y -= speed;
        if (key == sf::Keyboard::Down) pos.y += speed;
        if (key == sf::Keyboard::Left) pos.x -= speed;
        if (key == sf::Keyboard::Right) pos.x += speed;
        playerIcon.setPosition(pos);
        
        // 边界限制
        auto p = playerIcon.getPosition();
        p.x = std::clamp(p.x, 20.0f, 1000.0f);
        p.y = std::clamp(p.y, 20.0f, 740.0f);
        playerIcon.setPosition(p);
        
        // 交互
        if (key == sf::Keyboard::Enter) {
            checkBuildingInteraction();
        }
        
        // 菜单
        if (key >= sf::Keyboard::Num1 && key <= sf::Keyboard::Num9) {
            executeMenuOption(key - sf::Keyboard::Num1 + 1);
        }
        
        // 快捷键
        if (key == sf::Keyboard::S) {
            saveManager.save(stats, courseManager, skillManager, npcManager);
            addMessage("💾 存档成功");
        }
        if (key == sf::Keyboard::L) {
            if (saveManager.load(stats, courseManager, skillManager, npcManager)) {
                addMessage("💾 读档成功");
                stats.updateBuffs();
            } else {
                addMessage("❌ 读档失败");
            }
        }
        if (key == sf::Keyboard::R) {
            startInput("请输入新名字: ");
        }
        if (key == sf::Keyboard::E) {
            eventSystem.triggerRandomEvent(stats, courseManager, skillManager);
            addMessage("🎲 触发了随机事件！");
        }
        if (key == sf::Keyboard::Escape) {
            saveManager.save(stats, courseManager, skillManager, npcManager);
            window.close();
            isRunning = false;
        }
    }

    void startInput(const std::string& prompt) {
        isInputting = true;
        inputPrompt = prompt;
        inputBuffer.clear();
        addMessage("✏️ " + prompt);
    }

    void processInput() {
        isInputting = false;
        if (!inputBuffer.empty()) {
            stats.setName(inputBuffer);
            addMessage("✅ 名字已改为: " + inputBuffer);
        }
        inputBuffer.clear();
    }

    void checkBuildingInteraction() {
        auto pos = playerIcon.getPosition();
        for (auto& building : buildings) {
            auto bounds = building->getBounds();
            if (bounds.contains(pos.x, pos.y)) {
                std::cout << "\n🏛️ " << building->getEmoji() << " " 
                          << building->getName() << "\n";
                building->interact(stats, courseManager);
                
                // 更新GPA
                stats.setGPA(courseManager.calculateGPA());
                
                // 检查技能解锁
                if (stats.getGPA() >= 3.5 && !skillManager.hasSkill("学习基础")) {
                    skillManager.unlockSkill("学习基础");
                }
                
                // 自动存档
                saveManager.save(stats, courseManager, skillManager, npcManager);
                return;
            }
        }
        // 不在任何建筑附近
        std::cout << "🚶 在校园散步...\n";
    }

    void executeMenuOption(int option) {
        switch (option) {
            case 1: {  // 查看状态
                std::stringstream ss;
                ss << "📊 " << stats.getName() 
                   << " | 学期: " << stats.getSemester()
                   << " | GPA: " << std::fixed << std::setprecision(2) << stats.getGPA()
                   << " | 课程: " << courseManager.getCourseCount()
                   << " | 技能: " << skillManager.getSkillCount()
                   << " | 💰 " << stats.getMoney();
                addMessage(ss.str());
                break;
            }
            case 2: {  // 选修课程
                auto available = courseManager.getAvailableCourses();
                if (available.empty()) {
                    addMessage("❌ 没有更多课程可选");
                    break;
                }
                std::cout << "\n📚 可选课程:\n";
                for (size_t i = 0; i < available.size(); i++) {
                    std::cout << "  " << i + 1 << ". " << available[i].getName() 
                              << " (" << available[i].getCredit() << "学分)\n";
                }
                std::cout << "选择课程编号 (0取消): ";
                int choice;
                std::cin >> choice;
                if (choice > 0 && choice <= available.size()) {
                    if (courseManager.takeCourse(available[choice-1].getName())) {
                        addMessage("📖 选修了: " + available[choice-1].getName());
                        stats.modifyEnergy(-5);
                    } else {
                        addMessage("⚠️ 已选修该课程");
                    }
                }
                break;
            }
            case 3: {  // 学习备考
                auto courses = courseManager.getCourses();
                if (courses.empty()) {
                    addMessage("⚠️ 请先选修课程！");
                    break;
                }
                std::cout << "\n📝 选择要学习的课程:\n";
                for (size_t i = 0; i < courses.size(); i++) {
                    std::cout << "  " << i + 1 << ". " << courses[i].getName() 
                              << (courses[i].getScore() > 0 ? " (已考)" : " (未考)") << "\n";
                }
                std::cout << "选择课程编号 (0取消): ";
                int choice;
                std::cin >> choice;
                if (choice > 0 && choice <= courses.size()) {
                    int hours;
                    std::cout << "学习几个小时? (1-5): ";
                    std::cin >> hours;
                    hours = std::clamp(hours, 1, 5);
                    courseManager.studyForExam(courses[choice-1].getName(), hours, stats);
                    stats.setGPA(courseManager.calculateGPA());
                    addMessage("📝 学习了 " + courses[choice-1].getName());
                    
                    // 检查技能解锁
                    if (courses[choice-1].getScore() >= 85) {
                        std::vector<std::string> possible = {
                            "编程能力", "数学思维", "英语应用", "数据分析", "项目管理"
                        };
                        std::uniform_int_distribution<int> dist(0, possible.size()-1);
                        std::mt19937 rng(std::time(nullptr));
                        skillManager.unlockSkill(possible[dist(rng)]);
                    }
                }
                break;
            }
            case 4: {  // NPC交互
                auto& npcs = npcManager.getNPCs();
                std::cout << "\n💬 选择要互动的NPC:\n";
                for (size_t i = 0; i < npcs.size(); i++) {
                    std::cout << "  " << i + 1 << ". " << npcs[i].getName() 
                              << " (好感度: " << npcs[i].getFavorability() << ")\n";
                }
                std::cout << "选择NPC (0取消): ";
                int choice;
                std::cin >> choice;
                if (choice > 0 && choice <= npcs.size()) {
                    npcManager.interactWithNPC(choice-1, stats, courseManager);
                    addMessage("💬 与 " + npcs[choice-1].getName() + " 互动");
                    stats.setGPA(courseManager.calculateGPA());
                    saveManager.save(stats, courseManager, skillManager, npcManager);
                }
                break;
            }
            case 5: {  // 考试周
                if (courseManager.getCourses().empty()) {
                    addMessage("⚠️ 请先选修课程！");
                } else {
                    courseManager.examWeek(stats);
                    stats.setGPA(courseManager.calculateGPA());
                    addMessage("📝 考试周结束！");
                    saveManager.save(stats, courseManager, skillManager, npcManager);
                }
                break;
            }
            case 6: {  // 推进学期
                stats.nextSemester();
                stats.modifyEnergy(20);
                stats.modifyHappiness(10);
                stats.updateBuffs();
                stats.setGPA(courseManager.calculateGPA());
                addMessage("📅 进入第 " + std::to_string(stats.getSemester()) + " 学期");
                saveManager.save(stats, courseManager, skillManager, npcManager);
                break;
            }
            case 7: {  // 休息
                int bonus = stats.hasBuff("精力恢复+50%") ? 60 : 40;
                stats.modifyEnergy(bonus);
                addMessage("😴 休息了一天，精力 +" + std::to_string(bonus));
                break;
            }
            case 8: {  // 摸鱼
                stats.modifyHappiness(5);
                stats.modifyEnergy(-5);
                addMessage("🎮 摸鱼中... 快乐+5 精力-5");
                break;
            }
            case 9: {  // 尝试毕业
                if (stats.getSemester() >= 8 && 
                    stats.getGPA() >= 2.0 && 
                    courseManager.getCourseCount() >= 20) {
                    addMessage("🎉🎉🎉 恭喜毕业！！！");
                    addMessage("📊 最终GPA: " + std::to_string(stats.getGPA()));
                    addMessage("🏆 掌握技能: " + std::to_string(skillManager.getSkillCount()) + "个");
                    isRunning = false;
                } else {
                    std::string reason;
                    if (stats.getSemester() < 8) 
                        reason += "学期数不足(" + std::to_string(stats.getSemester()) + "/8) ";
                    if (stats.getGPA() < 2.0) 
                        reason += "GPA不足(" + std::to_string(stats.getGPA()) + "/2.0) ";
                    if (courseManager.getCourseCount() < 20) 
                        reason += "课程数不足(" + std::to_string(courseManager.getCourseCount()) + "/20)";
                    addMessage("❌ 不满足毕业条件: " + reason);
                }
                break;
            }
            default:
                addMessage("❌ 无效选项");
        }
        stats.setGPA(courseManager.calculateGPA());
        saveManager.save(stats, courseManager, skillManager, npcManager);
    }

    void addMessage(const std::string& msg) {
        messages.push_back(msg);
        if (messages.size() > 20) messages.erase(messages.begin());
        std::cout << msg << "\n";
    }

    void update() {
        // 检查游戏结束
        if (stats.getEnergy() <= 0) {
            addMessage("😵 精力耗尽，累倒了...");
            isRunning = false;
        }
        if (stats.getHappiness() <= 0) {
            addMessage("😢 太不开心了，退学了...");
            isRunning = false;
        }
        
        // 自动保存（每10秒）
        static int frameCount = 0;
        if (++frameCount % 600 == 0) {
            saveManager.save(stats, courseManager, skillManager, npcManager);
        }
    }

    void render() {
        window.clear(sf::Color(34, 139, 34));
        
        // 绘制地面网格
        for (int i = 0; i < 10; i++) {
            sf::RectangleShape line({1000, 1});
            line.setPosition(10, 100 + i * 60);
            line.setFillColor({255, 255, 255, 30});
            window.draw(line);
        }
        
        // 绘制建筑
        for (auto& building : buildings) {
            sf::RectangleShape rect(building->getBounds().getSize());
            rect.setPosition(building->getBounds().getPosition());
            rect.setFillColor(building->getColor());
            rect.setOutlineColor(sf::Color::White);
            rect.setOutlineThickness(1);
            window.draw(rect);
            
            // 建筑名称
            sf::Text text;
            text.setFont(font);
            text.setString(building->getEmoji() + " " + building->getName());
            text.setCharacterSize(14);
            text.setFillColor(sf::Color::White);
            text.setPosition(building->getBounds().left + 5, 
                            building->getBounds().top + building->getBounds().height + 5);
            window.draw(text);
        }
        
        // 绘制UI面板
        window.draw(statusPanel);
        drawStatusInfo();
        
        window.draw(menuPanel);
        drawMenu();
        
        window.draw(messagePanel);
        drawMessages();
        
        // 绘制玩家
        window.draw(playerIcon);
        sf::Text nameText;
        nameText.setFont(font);
        nameText.setString("🎓 " + stats.getName());
        nameText.setCharacterSize(14);
        nameText.setFillColor(sf::Color::White);
        auto pos = playerIcon.getPosition();
        nameText.setPosition(pos.x - 20, pos.y - 30);
        window.draw(nameText);
        
        // 底部提示
        sf::Text hint;
        hint.setFont(font);
        hint.setString("方向键移动 | Enter交互 | 1-9菜单 | S存档 | L读档 | R改名 | E事件 | ESC退出");
        hint.setCharacterSize(12);
        hint.setFillColor(sf::Color::White);
        hint.setPosition(320, 755);
        window.draw(hint);
        
        // 输入框
        if (isInputting) {
            sf::RectangleShape inputBox({400, 30});
            inputBox.setPosition(320, 720);
            inputBox.setFillColor({0, 0, 0, 200});
            inputBox.setOutlineColor(sf::Color::White);
            inputBox.setOutlineThickness(1);
            window.draw(inputBox);
            
            sf::Text inputText;
            inputText.setFont(font);
            inputText.setString("> " + inputBuffer + "_");
            inputText.setCharacterSize(16);
            inputText.setFillColor(sf::Color::White);
            inputText.setPosition(325, 722);
            window.draw(inputText);
        }
        
        window.display();
    }

    void drawStatusInfo() {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(15);
        text.setFillColor(sf::Color::White);
        
        std::stringstream ss;
        ss << "📊 " << stats.getName() << "\n"
           << "学期: " << stats.getSemester() << "\n"
           << "GPA: " << std::fixed << std::setprecision(2) << stats.getGPA() << "\n"
           << "课程: " << courseManager.getCourseCount() << "门\n"
           << "技能: " << skillManager.getSkillCount() << "个\n"
           << "💰 " << stats.getMoney() << "元\n";
        text.setString(ss.str());
        text.setPosition(20, 20);
        window.draw(text);
        
        // 进度条
        drawProgressBar(20, 155, 200, 10, stats.getEnergy(), sf::Color::Green, "精力");
        drawProgressBar(20, 180, 200, 10, stats.getHappiness(), sf::Color::Yellow, "快乐");
        
        // Buff显示
        int y = 205;
        if (stats.hasBuff("学习效率+20%")) {
            sf::Text buffText;
            buffText.setFont(font);
            buffText.setString("📈 学习效率+20%");
            buffText.setCharacterSize(12);
            buffText.setFillColor(sf::Color::Cyan);
            buffText.setPosition(20, y);
            window.draw(buffText);
            y += 20;
        }
        if (stats.hasBuff("精力恢复+50%")) {
            sf::Text buffText;
            buffText.setFont(font);
            buffText.setString("💪 精力恢复+50%");
            buffText.setCharacterSize(12);
            buffText.setFillColor(sf::Color::Cyan);
            buffText.setPosition(20, y);
            window.draw(buffText);
        }
    }

    void drawProgressBar(float x, float y, float width, float height, int value, sf::Color color, const std::string& label) {
        sf::RectangleShape bg({width, height});
        bg.setPosition(x, y);
        bg.setFillColor({80, 80, 80});
        window.draw(bg);
        
        float fill = std::clamp(value / 100.0f, 0.0f, 1.0f);
        sf::RectangleShape bar({width * fill, height});
        bar.setPosition(x, y);
        bar.setFillColor(color);
        window.draw(bar);
        
        sf::Text text;
        text.setFont(font);
        text.setString(label + ": " + std::to_string(value) + "%");
        text.setCharacterSize(12);
        text.setFillColor(sf::Color::White);
        text.setPosition(x, y - 15);
        window.draw(text);
    }

    void drawMenu() {
        std::vector<std::string> items = {
            "1. 📊 状态", "2. 📖 选课", "3. 📝 学习",
            "4. 💬 NPC", "5. 📝 考试周", "6. 📅 新学期",
            "7. 😴 休息", "8. 🎮 摸鱼", "9. 🎓 毕业"
        };
        
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(16);
        text.setFillColor(sf::Color::White);
        
        for (size_t i = 0; i < items.size(); i++) {
            text.setString(items[i]);
            text.setPosition(20, 250 + i * 35);
            window.draw(text);
        }
        
        sf::Text hint;
        hint.setFont(font);
        hint.setString("🎲 E = 随机事件");
        hint.setCharacterSize(13);
        hint.setFillColor(sf::Color::Yellow);
        hint.setPosition(20, 570);
        window.draw(hint);
    }

    void drawMessages() {
        sf::Text text;
        text.setFont(font);
        text.setCharacterSize(13);
        text.setFillColor(sf::Color::White);
        
        int start = std::max(0, (int)messages.size() - 6);
        for (int i = start; i < (int)messages.size(); i++) {
            text.setString(messages[i]);
            text.setPosition(330, 610 + (i - start) * 22);
            window.draw(text);
        }
    }
};

// ============================================================
// 10. main - 主函数
// ============================================================
int main() {
    std::cout << "🎓 学生生活模拟器 v3.0\n";
    std::cout << "========================================\n";
    std::cout << "✨ 新功能:\n";
    std::cout << "  🎲 随机事件系统\n";
    std::cout << "  💬 NPC好感度系统\n";
    std::cout << "  🏆 技能系统\n";
    std::cout << "  💾 JSON存档\n";
    std::cout << "  🏛️ 多态建筑交互\n";
    std::cout << "========================================\n\n";
    
    try {
        GameManager game;
        game.run();
    } catch (const std::exception& e) {
        std::cerr << "❌ 程序异常: " << e.what() << "\n";
        std::cout << "按任意键退出...\n";
        std::cin.get();
        return 1;
    }
    
    std::cout << "游戏已结束！\n";
    return 0;
}
