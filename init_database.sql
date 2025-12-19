CREATE TABLE IF NOT EXISTS players (
    name VARCHAR(50) PRIMARY KEY,
    password_hash VARCHAR(256) NOT NULL DEFAULT '',
    total_score INTEGER DEFAULT 0,
    quizzes_completed INTEGER DEFAULT 0,
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    last_played TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS achievements (
    name VARCHAR(50) NOT NULL,
    achievement_id VARCHAR(50) NOT NULL,
    unlock_date TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (name) REFERENCES players(name) ON DELETE CASCADE,
    UNIQUE(name, achievement_id)
);

CREATE TABLE IF NOT EXISTS quiz_results (
    name VARCHAR(50) NOT NULL,
    score INTEGER NOT NULL,
    correct_answers INTEGER NOT NULL,
    total_questions INTEGER NOT NULL,
    category VARCHAR(50),
    FOREIGN KEY (name) REFERENCES players(name) ON DELETE CASCADE
);

CREATE INDEX IF NOT EXISTS idx_players_score ON players(total_score DESC);
CREATE INDEX IF NOT EXISTS idx_quiz_results_name ON quiz_results(name);
CREATE INDEX IF NOT EXISTS idx_achievements_name ON achievements(name);

SELECT 'База данных AstroLearn инициализирована успешно!' as message;
