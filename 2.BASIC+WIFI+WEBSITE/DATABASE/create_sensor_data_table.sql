DROP TABLE IF EXISTS sensor_data CASCADE;

CREATE TABLE IF NOT EXISTS sensor_data (
    id SERIAL PRIMARY KEY,
    sensor_value TEXT NOT NULL,
    created_at TIMESTAMP WITH TIME ZONE DEFAULT now()
);

ALTER TABLE sensor_data ENABLE ROW LEVEL SECURITY;

-- Allow all (public) select/insert for anon key (for demo/dev, restrict in production!)
CREATE POLICY "Allow read for all"
  ON sensor_data
  FOR SELECT
  USING (true);

CREATE POLICY "Allow insert for all"
  ON sensor_data
  FOR INSERT
  WITH CHECK (true);
